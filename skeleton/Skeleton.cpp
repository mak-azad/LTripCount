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
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
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
      //int lc[20]={0};
      std::map<const llvm::Loop*, int> lmap2;
       std::map<std::string, bool> lprocessed;
      std::string functionName;

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
      int flag[10] = {0};

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
              //Value * args[] = {LoadVal,lc[lcounter]};
              //Builder.CreateCall(logFunc,args);
             // if (lc[lcounter] == 0)
            // errs() << "L:" << lprocessed[ld] << "\n";
            // if (lprocessed[ld] != true){
              Value *NewValueL = ConstantInt::get(IntTy, lcounter);
              Value *Loc;
             // errs() << L->getStartLoc().getLine() << "\n";
              DebugLoc debugLoc = L->getStartLoc();
              if (debugLoc) {
                  errs() << "Loop starts at line " << debugLoc.getLine() << "\n";
                  
                  unsigned int location = debugLoc.getLine() ;
                  Loc = ConstantInt::get(IntTy, location);
                 // errs() << location << "\n";
                  functionName = F.getName().str();
                  errs() << functionName << "\n";
              }
              //
              // LLVMContext context;
             // IRBuilder<> builder(context);
              //builder.SetInsertPoint(&BB, ++builder.GetInsertPoint());
             Value *valStr = Builder.CreateGlobalStringPtr(functionName.c_str());
             // std::string myString = functionName;
            errs() << "From:" << valStr << "\n";
             
              // IRBuilder<> builder(context);
              // std::string myString = functionName;

              // // Create a GlobalVariable that represents the string value of myString
              // GlobalVariable* stringValue = builder.CreateGlobalStringPtr(myString);

// Assign the address of the GlobalVariable to a Value* pointer
              //Value* stringValue = stringGlobal;
              //  Type* charType = Type::getInt8Ty(context);
              //  Module *module = F.getParent();
              // Constant* stringConstant = ConstantArray::get(context, functionName.c_str(), true);
              // Create a ConstantArray that represents the string value of myString
              //Constant* stringConstant = ConstantDataArray:&:getString(context, myString, true);

              // Assign the address of the ConstantArray to a Value* pointer
              //Value* stringValue = stringConstant;
              // Create a global variable that references the string constant
              // GlobalVariable* globalString = new GlobalVariable(*module, stringConstant->getType(), true, GlobalValue::PrivateLinkage, stringConstant);
              // globalString->setName("myString");

              // Use the address of the global variable as the Value* pointer
             // Value* stringValue = globalString;
              //Value* stringValue = new GlobalVariable(*module, stringConstant->getType(), true, GlobalValue::PrivateLinkage, stringConstant);
              //() << "Again:" << stringValue << "\n";
                            //

              Value* args[] = {LoadVal, NewValueL, Loc, valStr};
              //Value* args[] = {LoadVal,Loc,NewValueL,stringValue};
              Builder.CreateCall(logFunc,args);
              flag[lcounter] = 1;
            //  lprocessed[ld] = true;
             //}
              //lc[lcounter] = 1;
              Constant *NewValue = ConstantInt::get(IntTy, 0);
             // for(int x = 0; x < rcnt; x++)
              //  Builder.CreateStore(NewValue, Counter[x])
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
