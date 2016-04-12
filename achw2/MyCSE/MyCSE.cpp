#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/Loads.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/PHITransAddr.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/SSAUpdater.h"
#include <vector>
#define DEBUG_TYPE "mycse"
using namespace llvm;
using namespace PatternMatch;

//===------------------------------------------------------------===//
//					  Expression Class
//===-----------------------------------------------------------===//


namespace{
	struct Expression{
		uint32_t opcode;
		Type * type;
		SmallVector<uint32_t,4> varargs;
		Expression(uint32_t o = ~2U) : opcode(o) {}
		bool operator==(const Expression &other) const{
			if(opcode != other.opcode)
				return false;
			if(opcode == ~0U || opcode == ~1U)
				return true;
			if(type != other.type)
				return false;
			if(varargs != other.varargs)
				return false;
			return true;
		}
		friend hash_code hash_value(const Expression &Value){
			return hash_combine(Value.opcode,Value.type,hash_combine_range(Value.varargs.begin(),
																		   Value.varargs.end()));
		}
	};
	class ValueTable{
		DenseMap<Value *,uint32_t> valueNumbering;
		DenseMap<Expression,uint32_t> expressionNumbering;
		//AliasAnalysis * AA;
		//MemoryDependenceAnalysis *MD;
		//DominatorTree *DT;
		
		uint32_t nextValueNumber;
		Expression create_expression(Instruction * I);
		Expression create_cmp_expression(unsigned Opcode, CmpInst::Predicate Predicate,
										 Value *LHS, Value * RHS);
		Expression create_extractvalue_expression(ExtractValueInst * EI);
		uint32_t lookup_or_add_call(CallInst * C);
		public:
			ValueTable(): nextValueNumber(1){}
			uint32_t lookup_or_add(Value *V);
			uint32_t lookup(Value*) const;
			uint32_t lookup_or_add_cmp(unsigned Opcode,CmpInst::Predicate Predicate,
									   Value * LHS,Value * RHS);

			void add(Value * V,uint32_t num);
			void clear();
			void erase(Value *V);
			//void setAliasAnalysis( AliasAnalysis *A){ AA = A;}
			//AliasAnalysis *getAliasAnalysis() const {return AA;}
			//void setMemDep(MemoryDependenceAnalysis *M) {MD = M; }
			//void setDomTree(DominatorTree * D) {DT = D; }
			uint32_t getNextUnusedValueNumber(){return nextValueNumber;}
			void verifyRemoved(const Value *) const;
	};
}
namespace llvm{
	template<> struct DenseMapInfo<Expression>{
		static inline Expression getEmptyKey(){
			return ~0U;
		}
		static inline Expression getTombstoneKey(){
			return ~1U;
		}
		static unsigned getHashValue(const Expression e){
			//using llvm:hash_value;
			return static_cast<unsigned>(hash_value(e));
		}
		static bool isEqual(const Expression &LHS, const Expression &RHS){
			return LHS == RHS;
		}
	};
	//void initializeMyCSEPass(PassRegistry &P);
	FunctionPass * createMyCSEPass(bool NoLoads);
}
void ValueTable::clear(){
	valueNumbering.clear();
	expressionNumbering.clear();
	nextValueNumber = 1;
}
void ValueTable::erase(Value *V){
	valueNumbering.erase(V);
}
Expression ValueTable::create_extractvalue_expression(ExtractValueInst * EI){
	Expression e;
	e.type = EI->getType();
	e.opcode = EI->getOpcode();
	for(Instruction::op_iterator OI= EI->op_begin(), OE = EI->op_end(); OI!=OE;++OI){
		e.varargs.push_back(lookup_or_add(*OI));
	}
	for(ExtractValueInst::idx_iterator II= EI->idx_begin(), IE = EI->idx_end(); II != IE; ++II)
		e.varargs.push_back(*II);
	return e;
}
Expression ValueTable::create_expression(Instruction *I){
	Expression e;
	e.type = I->getType();
	e.opcode = I->getOpcode();
	for(Instruction::op_iterator OI= I->op_begin(), OE = I->op_end(); OI!=OE;++OI){
		e.varargs.push_back(lookup_or_add(*OI));
	}
	if(I->isCommutative()){
		if(e.varargs[0] > e.varargs[1])
			std::swap(e.varargs[0] ,e.varargs[1]);
	}
	if(CmpInst * C = dyn_cast<CmpInst>(I)){
		CmpInst::Predicate Predicate = C->getPredicate();
		if(e.varargs[0] > e.varargs[1]){
			std::swap(e.varargs[0], e.varargs[1]);
			Predicate = CmpInst::getSwappedPredicate(Predicate);
		}
		e.opcode = (C->getOpcode() <<8) | Predicate;
	}else if(InsertValueInst *E = dyn_cast<InsertValueInst>(I)){
		for(InsertValueInst::idx_iterator II = E->idx_begin(), IE = E->idx_end(); II != IE; II++)
			e.varargs.push_back(*II);		
	}
	return e;
}


uint32_t ValueTable::lookup_or_add(Value *V ){
	DenseMap<Value*,uint32_t>::iterator VI = valueNumbering.find(V);
	if(VI != valueNumbering.end())
		return VI->second;
	if(!isa<Instruction>(V)){
		valueNumbering[V] = nextValueNumber;
		return nextValueNumber++;
	}
	Instruction *I = cast<Instruction>(V);
	Expression exp;
	switch(I->getOpcode()){
		case Instruction::Add:
		case Instruction::FAdd:
		case Instruction::Sub:
		case Instruction::FSub:
		case Instruction::Mul:
		case Instruction::FMul:
		case Instruction::UDiv:
		case Instruction::SDiv:
		case Instruction::FDiv:
		case Instruction::URem:
		case Instruction::SRem:
		case Instruction::FRem:
		case Instruction::Shl:
		case Instruction::And:
		case Instruction::Or:
		case Instruction::Xor:
		case Instruction::Select:
		case Instruction::ExtractElement:
		case Instruction::InsertValue:
		case Instruction::GetElementPtr:
			exp = create_expression(I);
			break;
		case Instruction::ExtractValue:
			exp = create_extractvalue_expression(cast<ExtractValueInst>(I));
			break;
		default:
			valueNumbering[V]=nextValueNumber;
			return nextValueNumber++;
	}
	uint32_t&e = expressionNumbering[exp];
	if(!e) e = nextValueNumber++;
	valueNumbering[V]= e;
	return e;
	return 0;
}
//===--------------------------------------------------------===//
//				My CSE Pass
//===--------------------------------------------------------===//
class MyCSE : public FunctionPass {
	bool NoLoads;
	MemoryDependenceAnalysis *MD;
	DominatorTree *DT;
	const TargetLibraryInfo *TLI;
	AssumptionCache * AC;
	SetVector<BasicBlock * > DeadBlocks;
	ValueTable VN;
	SmallVector<Instruction*,8>InstrsToErase;
	struct LeaderTableEntry{
		Value *Val;
		const BasicBlock *BB;
		LeaderTableEntry *Next;
	};
	DenseMap<uint32_t,LeaderTableEntry> LeaderTable;
	BumpPtrAllocator TableAllocator;
	public:
		static char ID;
		//MyCSE() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override;
		explicit MyCSE():FunctionPass(ID){}
		void markInstructionForDeletion(Instruction *I){
			VN.erase(I);
			InstrsToErase.push_back(I);
		}
	private:
	
	bool iterateOnFunction(Function &F);
	bool processBlock(BasicBlock* BB);
	bool processInstruction(Instruction *I);
	void cleanupGlobalSets();
	bool processLoad(LoadInst *L);
};
char MyCSE::ID =0;
static RegisterPass<MyCSE> X("myMagicCSE" , "My CSE Pass" , false , false );

static void patchAndReplaceAllUsesWith(Instruction *I, Value *Repl){
	I->replaceAllUsesWith(Repl);
}
bool MyCSE::processLoad(LoadInst * L){
	if(!L->isSimple())
		return false;
	if(L->use_empty()){
		markInstructionForDeletion(L);
	}
	return false;
}
bool MyCSE::runOnFunction(Function &F){
	if(skipOptnoneFunction(F))
		return false;
	
	unsigned Iteration =0 ;
	bool hasChange = true;
	bool changed  = false;
	bool onceChanged = false;
	while(hasChange){
		hasChange= false;
		for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE ; BI++){
			do{
				changed = processBlock(BI);
				hasChange |= changed;
			}while(changed);	
		}
		onceChanged |= hasChange;
	}
	return changed;

};
/*FunctionPass *llvm::createMyCSEPass(bool NoLoads){
	return new MyCSE(NoLoads);
}*/
void MyCSE::cleanupGlobalSets(){
	VN.clear();
	LeaderTable.clear();
	TableAllocator.Reset();
}
bool MyCSE::processInstruction(Instruction *I){
	//I->print(errs()); 
	if(isa<DbgInfoIntrinsic>(I))
		return false;
	if(LoadInst *LI = dyn_cast<LoadInst>(I)){
		if(processLoad(LI))
			return true;
		unsigned Num = VN.lookup_or_add(LI);
		return false;
	}
	/*if(BranchInst* BI = dyn_cast<BranchInst>(I)){
		if(!BI->isConditional())
			return false;
		if(isa<Constant>(BI->getCondition()))
			return processFoldableCondBr(BI);
		return false;
	}*/
	if(I->getType()->isVoidTy()) return false;
	uint32_t NextNum = VN.getNextUnusedValueNumber();
	unsigned Num = VN.lookup_or_add(I);
	if(isa<AllocaInst>(I) || isa<TerminatorInst>(I) || isa<PHINode>(I)){
		return false;
	}
	if(Num >= NextNum){
		return false;
	}
	Value * repl ;//= findLeader(I->getParent() , Num);
	if(!repl){
		//addToLeaderTable(Num,I,I->getParent());
		return false;
	}
	patchAndReplaceAllUsesWith(I,repl);
	if(MD && repl->getType()->getScalarType()->isPointerTy())
		MD->invalidateCachedPointerInfo(repl);
	markInstructionForDeletion(I);
	return true;
}
bool MyCSE::processBlock(BasicBlock *BB){
	bool changed = false;
	for(BasicBlock::iterator II = BB->begin() , IE = BB->end(); II!=IE;){
		changed |= processInstruction(II);
		if(InstrsToErase.empty()){
			II++;
			continue;
		}
		//NumGVNInstr += InstrsToErase.size();
		bool AtStart = II ==BB->begin();
		if(!AtStart)
			--II;
		for(SmallVectorImpl<Instruction *>::iterator I = InstrsToErase.begin(),E= InstrsToErase.end();
			I!=E ; ++I){
			DEBUG(dbgs()<<"GVN removed: "<<**I<<'\n');
			if(MD) MD->removeInstruction(*I);
			(*I)->eraseFromParent();	
		}
		InstrsToErase.clear();
		if(AtStart)
			II=BB->begin();
		else
			++II;
	}
	return changed;
}
