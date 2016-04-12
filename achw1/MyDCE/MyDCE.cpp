#include "MyDCE.h"
char MyDCE::ID =0;
static RegisterPass<MyDCE> X("myMagicDCE" , "My DCE Pass" , false , false );
STATISTIC(MyDCEEliminated,"Number of insts removed");
bool MyDCE::runOnFunction(Function &F){
	if(skipOptnoneFunction(F))
		return false;
	SmallVector<Instruction *,16> WorkList;

	for (auto b = F.begin(); b!= F.end () ; ++b){
		BasicBlock &BB = *b;
		for (auto i = BB.begin() ; i != BB.end(); ++i){
			WorkList.push_back(&*i);
		}
	}
	bool MadeChange = false;
	while(!WorkList.empty()){
		Instruction * I = WorkList.pop_back_val();
		if(isDeadInstruction(I)){
			errs() << "removing somebody " << *I <<"\n";
			for (unsigned int i =0 , e = I->getNumOperands(); i !=e  ; i++){
				Value * OpV = I->getOperand(i);
				I->setOperand(i,nullptr);
				if(!OpV->use_empty())
					continue;
				if(Instruction *  OI = dyn_cast<Instruction>(OpV)){
					WorkList.push_back(OI);	
				}
			}
			/*for(User::op_iterator OI = I->op_begin(), E = I->op_end(); OI != E; OI++){
			
				if(Instruction * Used = dyn_cast<Instruction>(*OI)){
					WorkList.push_back(Used);
				}
			}*/
			I->eraseFromParent();
			WorkList.erase(std::remove(WorkList.begin(),WorkList.end(),I),
						WorkList.end());
			MadeChange = true;
			++MyDCEEliminated;
		}	
	}
	return MadeChange;
};
bool MyDCE::isDeadInstruction(Instruction * I){
	if(!I->use_empty() || isa<TerminatorInst>(I)) return false;
	
	if(isa<LandingPadInst>(I))
		return false;
	if(DbgDeclareInst * DDI = dyn_cast<DbgDeclareInst>(I))
		return !DDI->getAddress();	
	
	if(DbgValueInst *DVI = dyn_cast<DbgValueInst>(I))
		return !DVI->getValue();
	if(!I->mayHaveSideEffects()) return true;
	
	// special cases may have side effect
	if(IntrinsicInst *II = dyn_cast<IntrinsicInst>(I)){
		if(II->getIntrinsicID() == Intrinsic::lifetime_start || 
			II->getIntrinsicID() == Intrinsic::lifetime_end)
				return isa<UndefValue>(II->getArgOperand(1));
		if(II->getIntrinsicID() == Intrinsic::stacksave)
			return true;
		
		if(II->getIntrinsicID() == 	Intrinsic::assume){
			if(ConstantInt *Cond = dyn_cast<ConstantInt>(II->getArgOperand(0)))
				return !Cond->isZero();
			return false;
		}
	}
	
	return false;	
}
