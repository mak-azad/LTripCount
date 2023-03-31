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
#include <map>
#include <vector>
#include <string>
using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    
    virtual bool runOnFunction(Function &F) {
      bool modified = false;
      // Insert a local variable outside the loop
      Type *IntTy = Type::getInt32Ty(F.getContext());
      
      // }
      //AllocaInst *Counter = new AllocaInst(IntTy, 0, "my_local", F.getEntryBlock().getFirstNonPHI());
      std::map<const llvm::Loop*, bool> lmap;
      std::map<const llvm::Loop*, int> lmap2;
       std::map<const llvm::Loop*, bool> lprocessed;

      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee logFunc = 
      F.getParent()->getOrInsertFunction("logop", logFuncType);

      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

      int cnt, lcnt, rcnt=0;
      // unsigned int level =-1;
       for (auto &BB : F) {
           auto *L = LI.getLoopFor(&BB);
           
           cnt = -1;
           if(L){

            // errs() << LoopName << "\n";
             lcnt = 1;
               while (L->getParentLoop()) {

                   L = L->getParentLoop();
                   lcnt++;
                  // errs () << lcnt ;
                }
              if (lcnt > rcnt){
                   cnt = lcnt;
                   rcnt = cnt;
              }
               
           }
       }
       errs () << "Max nested depth:"<< rcnt << "\n";

       // // use different counters for nested loops
       std::vector<AllocaInst *> Counter(10);
        for(int i = 0; i < 10; i++){
            std::string t = std::to_string(i);
            t = "my_local_"+t;
            Counter[i] =  new AllocaInst(IntTy, 0, t, F.getEntryBlock().getFirstNonPHI());
            IRBuilder<> Builder(F.getEntryBlock().getFirstNonPHI());
            Constant *NewValue = ConstantInt::get(IntTy, 0);
            Builder.CreateStore(NewValue, Counter[i]);
       }


      // errs() << "MXLEVEL:" << level << "\n";


     // errs() << "Nested loop  depth count:" << maxDepth << "\n";

      Loop *PL = nullptr;
      

      int lcounter = 0;

      for (auto &BB : F) {
        bool loopModified = false;
        auto *L = LI.getLoopFor(&BB);
        //lmap[L] = false;
        //lprocessed[L] = false;


        Value *IncVal = nullptr;
        //auto it = lmap.find(L);


        // we use seperate counter for each detected loop, consider 

        if(L && lmap[L]!= true){
          //PL = L;
          // map to store the traveresd loop
          lmap[L] = true;
         
          errs() << "MyPass: ";
          modified = true;
          loopModified = true;
          // Get the loop backedge block and the loop latch instruction
          BasicBlock *BackedgeBB = L->getLoopLatch();
          Instruction *LatchInst = BackedgeBB->getTerminator();

          IRBuilder<> Builder(LatchInst);
          // Increment the counter at each loop 

          std::string tmp = std::to_string(lcounter);
          std::string ld = "my_local_"+tmp + ".load";
          std::string inc = "my_local_" + tmp + ".inc";

          Value *LoadVal = Builder.CreateLoad(Builder.getInt32Ty(), Counter[lcounter], ld);
          IncVal = Builder.CreateAdd(LoadVal, ConstantInt::get(IntTy, 1), inc);
          //Value *LoadVal = Builder.CreateLoad(Builder.getInt32Ty(), Counter[0], "my_local_0.load");
          //IncVal = Builder.CreateAdd(LoadVal, ConstantInt::get(IntTy, 1), "my_local_0.inc");
          Builder.CreateStore(IncVal, Counter[lcounter]);
          //Builder.CreateCall(logFunc, IncVal);
          if(loopModified)
          {
            //lprocessed[L] = true;
            SmallVector<BasicBlock*, 8> ExitBlocks;
            L->getExitBlocks(ExitBlocks);
           // errs() << "EB Size for this loop:" <<itBlocks.size() << "\n";
            if (ExitBlocks.empty())
              break;
            for (unsigned i = 0, e = ExitBlocks.size(); i != e; ++i){
              Instruction *exitInst = ExitBlocks[i]->getTerminator();
              IRBuilder<> Builder(exitInst);
              //new by me
              //errs() << lcounter << "\n";
                //std::string tmp1 = std::to_string(lcounter);
               // std::string ld1 = "my_local_"+tmp1 + ".load";
               // std::string inc1 = "my_local_" + tmp1 + ".inc";
              Value *LoadVal = Builder.CreateLoad(Builder.getInt32Ty(), Counter[lcounter], ld);
              //Builder.CreateCall(logFunc, IncVal);
              //errs() << "EB Size for this loop:" << ExitBlocks.size() << "\n";
              //errs() << "Printing for counter:" << *(Counter[lcounter]) << "\n";
              Builder.CreateCall(logFunc,{LoadVal});
              Constant *NewValue = ConstantInt::get(IntTy, 0);
             // for(int x = 0; x < rcnt; x++)
              //  Builder.CreateStore(NewValue, Counter[x]);
              //errs() << "Printing for counter:" << lcounter << "\n";
              Builder.CreateStore(NewValue, Counter[lcounter]);
            }
            lcounter++;
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
    // unsigned int getMaxNestedLoopLevel(llvm::Loop* L) {
    //   unsigned int maxLevel = 0;
    //   for (llvm::Loop::iterator LI = L->begin(), LE = L->end(); LI != LE; ++LI) {
    //       llvm::Loop* SubLoop = *LI;
    //       unsigned int level = getMaxNestedLoopLevel(SubLoop) + 1;
    //       if (level > maxLevel) {
    //           maxLevel = level;
    //       }
    //   }
    //   return maxLevel;
    // }
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
