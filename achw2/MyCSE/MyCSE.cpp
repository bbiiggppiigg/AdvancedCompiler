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
STATISTIC(NumGVNInstr,  "Number of instructions deleted");
STATISTIC(NumGVNLoad,   "Number of loads deleted");
STATISTIC(NumGVNPRE,    "Number of instructions PRE'd");
STATISTIC(NumGVNBlocks, "Number of blocks merged");
STATISTIC(NumGVNSimpl,  "Number of instructions simplified");
STATISTIC(NumGVNEqProp, "Number of equalities propagated");
STATISTIC(NumPRELoad,   "Number of loads PRE'd");

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
		AliasAnalysis * AA;
		MemoryDependenceAnalysis *MD;
		DominatorTree *DT;
		
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
			void setAliasAnalysis( AliasAnalysis *A){ AA = A;}
			AliasAnalysis *getAliasAnalysis() const {return AA;}
			void setMemDep(MemoryDependenceAnalysis *M) {MD = M; }
			void setDomTree(DominatorTree * D) {DT = D; }
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
	void initializeMyCSEPass(PassRegistry &P);
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
	public:
		static char ID;
		//MyCSE() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override;
		explicit MyCSE(bool noloads=false):FunctionPass(ID),NoLoads(noloads),MD(nullptr){
			initializeMyCSEPass(*PassRegistry::getPassRegistry());
		
		}
	private:
	bool iterateOnFunction(Function &F);
	bool processBlock(BasicBlock* BB);
	bool processInstruction(Instruction *I);
	void cleanupGlobalSets();
};
char MyCSE::ID =0;
static RegisterPass<MyCSE> X("myMagicCSE" , "My CSE Pass" , false , false );

INITIALIZE_PASS_BEGIN(MyCSE, "myMagicCSE", "My CSE Pass", false, false)
INITIALIZE_PASS_DEPENDENCY(AssumptionCacheTracker)
INITIALIZE_PASS_DEPENDENCY(MemoryDependenceAnalysis)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)
INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_END(MyCSE, "myMagicCSE", "My CSE Pass", false, false)

bool MyCSE::runOnFunction(Function &F){
	if(skipOptnoneFunction(F))
		return false;
	if (!NoLoads)
	    MD = &getAnalysis<MemoryDependenceAnalysis>();
	
	DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
	AC = &getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
	TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();
	VN.setAliasAnalysis(&getAnalysis<AliasAnalysis>());
	VN.setMemDep(MD);
	VN.setDomTree(DT);
	
	unsigned Iteration =0 ;
	bool hasChange = true;
	bool changed  = false;
	while(hasChange){
		DEBUG(dbgs() << "GVN iteration : "<<Iteration <<"\n");
		hasChange = iterateOnFunction(F);
		changed |= hasChange;
		++Iteration;
	}
	return changed;

};
FunctionPass *llvm::createMyCSEPass(bool NoLoads){
	return new MyCSE(NoLoads);
}
void MyCSE::cleanupGlobalSets(){
	VN.clear();
}
bool MyCSE::iterateOnFunction(Function &F){
//	VN.clear();
	bool changed = false;
	std::vector<BasicBlock*> BBs;
	ReversePostOrderTraversal<Function *> RPOT(&F);
	for(ReversePostOrderTraversal<Function *>::rpo_iterator RI = RPOT.begin(), RE= RPOT.end(); RI!= RE ; ++RI)
		BBs.push_back(*RI);
	for(std::vector<BasicBlock*>::iterator I = BBs.begin(), E = BBs.end();I!=E ; I++)
		changed = processBlock(*I);
	return changed;
}
bool MyCSE::processInstruction(Instruction *I){
	if(isa<DbgInfoIntrinsinc>(I))
		return false;
	const DataLayout &DL = I->getModule()->getDataLayout();
	if(Value *V = SimplifyInstruction(I,DL,TLI,DT,AC)){
		I->replaceAllUsesWith(V);
		if(MD && V->getType()->getScalarType()->isPointerTy())
			MD->invalidateCachedPointerInfo(V);
		markInstructionForDeletion(I);
		++NumGVNSimpl;
		return true;
	}
	if(LoadInst *LI = dyn_cast<LoadInst>(I)){
		if(processLoad(LI))
			return true;
		unsigned Num = VN.lookup_or_add(LI);
		return false;
	}
	if(BranchInst* BI = dyn_cast<BranchInst>(I)){
		if(!BI->isConditional())
			return false;
		if(isa<Constant>(BI->getCondition()))
			return processFoldableCondBr(BI);
		return false;
	}
	if(I->getType()->VoidTy()) return false;
	uint32_t NextNum = Vn.getNextUnusedValueNumber();
	unsigned NUm = VN.lookup_or_add(I);
	if(isa<AllocaInst>(I) || isa<TerminatorInst>(I) || isa<PHINode>(I)){
		return false;
	}
	if(Num >= NextNum){
		return false;
	}
	return false;
}
bool MyCSE::processBlock(BasicBlock *BB){
	bool changed = false;
	for(BasicBlock::iterator BI = BB->begin() , BE = BB->end(); BI!=BE;){
		changed |= processInstruction(BI);
		if(InstrsToErase.empty()){
			BI++;
			continue;
		}
		NumGVNInstr += InstrsToErase.size();
		bool AtStart = BI ==BB->begin();
		if(!AtStart)
			--BI;
		for(SmallVectorImpl<Instruction *>::iterator I = InstrsToErase.begin(),E= InstrsToErase.end();
			I!=E ; ++I){
			DEBUG(dbgs()<<"GVN removed: "<<**I<<'\n');
			if(MD) MD->removeInstruction(*I);
			(*I)->eraseFromParent();	
		}
		InstrsToErase.clear();
		if(AtStart)
			BI=BB->begin();
		else
			++BI;
	}
	return changed;
}
