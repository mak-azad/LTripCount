#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      bool modified = false;
      // Insert a local variable outside the loop
      Type *IntTy = Type::getInt32Ty(F.getContext());
      AllocaInst *Counter = new AllocaInst(IntTy, 0, "my_local", F.getEntryBlock().getFirstNonPHI());

      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee logFunc = 
      F.getParent()->getOrInsertFunction("logop", logFuncType);

      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      Loop *PL = nullptr;
      for (auto &BB : F) {
        bool loopModified = false;
        auto *L = LI.getLoopFor(&BB);
        Value *IncVal = nullptr;
        if(L && L != PL){
          PL = L;
          errs() << "MyPass: ";
          modified = true;
          loopModified = true;
          // Get the loop backedge block and the loop latch instruction
          BasicBlock *BackedgeBB = L->getLoopLatch();
          Instruction *LatchInst = BackedgeBB->getTerminator();

          IRBuilder<> Builder(LatchInst);
          // Increment the counter at each loop 
          Value *LoadVal = Builder.CreateLoad(Builder.getInt32Ty(), Counter, "my_local.load");
          IncVal = Builder.CreateAdd(LoadVal, ConstantInt::get(IntTy, 1), "my_local.inc");
          Builder.CreateStore(IncVal, Counter);
          //Builder.CreateCall(logFunc, IncVal);
        }
        if(loopModified)
        {
          SmallVector<BasicBlock*, 8> ExitBlocks;
          L->getExitBlocks(ExitBlocks);
  
          if (ExitBlocks.empty())
            break;
          for (unsigned i = 0, e = ExitBlocks.size(); i != e; ++i){
            Instruction *exitInst = ExitBlocks[i]->getTerminator();
            IRBuilder<> Builder(exitInst);
            //new by me
            Value *LoadVal = Builder.CreateLoad(Builder.getInt32Ty(), Counter, "my_local.load");
            //Builder.CreateCall(logFunc, IncVal);
            Builder.CreateCall(logFunc,{LoadVal});
            Constant *NewValue = ConstantInt::get(IntTy, 0);
             Builder.CreateStore(NewValue, Counter);
          }
          
          // // // reinit the counter?
          // Constant *NewValue = ConstantInt::get(IntTy, 0);
          // IRBuilder<> Builder(&*(F.getEntryBlock().getFirstNonPHI()->getIterator())); // Insert instruction at the beginning of the entry block
          // Builder.CreateStore(NewValue, Counter);
          
        }
      }
        //Builder.CreateCall(logFunc, IncVal);
      return modified;
    }
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<llvm::LoopInfoWrapperPass>();
      //AU.addRequired<llvm::ScalarEvolutionWrapperPass>();
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
