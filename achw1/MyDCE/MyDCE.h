#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IntrinsicInst.h"
using namespace llvm;
#define DEBUG_TYPE "mydce"
class MyDCE : public FunctionPass {
	public:
		static char ID;
		MyDCE() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override;
	private:
		bool isDeadInstruction(Instruction *);
};
