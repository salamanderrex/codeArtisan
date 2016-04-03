// CSE 231 Project Part 1
// Profiling Branch Bias
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"

#include <iostream>

using namespace std;
using namespace llvm;

namespace {
    struct BranchBias : public ModulePass {
        static char ID;

        Function *BranchTotal;
        Function *BranchTaken;
        Function *PrintStats;
        Function *f_current;

        BranchBias() : ModulePass (ID) {}

        virtual bool runOnModule (Module &M) {
            // Initialize void BranchTaken(char* function)
            Constant* c1 = M.getOrInsertFunction("_Z11BranchTakenPc", 
                                           Type::getVoidTy(M.getContext()), 
                                           Type::getInt8PtrTy(M.getContext()), 
                                           (Type*)0);
            BranchTaken = cast<Function>(c1);
            // Initialize void BranchTotal(char* function)
            Constant* c2 = M.getOrInsertFunction("_Z11BranchTotalPc", 
                                           Type::getVoidTy(M.getContext()), 
                                           Type::getInt8PtrTy(M.getContext()), 
                                           (Type*)0);
            BranchTotal = cast<Function>(c2);
            // Initialize void PrintStats()
            Constant* c3 = M.getOrInsertFunction("_Z10PrintStatsv", 
                                           Type::getVoidTy(M.getContext()), 
                                           (Type*)0);
            PrintStats = cast<Function>(c3);

            // Run over Module
            for(Module::iterator itr = M.begin(), e = M.end(); itr!= e; ++itr) {
                StringRef f_name;
                f_name = (*itr).getName();
                f_current = itr;
                
                for(Function::iterator BB = itr->begin(), itr_e = itr->end(); BB != itr_e; ++BB)
                {
                    Value *f_name_str;
                    bool first = true;
                    // Run over the Basic Block
                    for(BasicBlock::iterator BB_itr = BB->begin(), BB_itr_e = BB->end(); BB_itr != BB_itr_e; ++BB_itr)
                    {
                        // Check if Branch Instructions:
                        if(isa<BranchInst>(&(*BB_itr)) ) {
                            BranchInst *branIst = dyn_cast<BranchInst>(BB_itr);
                            // Check if Conditional Branches
                            if (branIst->isConditional()) {
                                //Create a new block
                                BasicBlock *block_next = BasicBlock::Create(getGlobalContext(), "block_next", f_current);
                                IRBuilder<> builder(block_next);

                                // Check if the first block
                                if (first) {
                                    // if it is the first block, get fucntion name (note: global!!!)
                                    f_name_str = builder.CreateGlobalStringPtr(f_name, "f_name_str");
                                    first = false;
                                }

                                // Create a call to BranchTaken
                                builder.CreateCall (BranchTaken, f_name_str);

                                // Get and add taken block
                                BasicBlock *block_taken = branIst->getSuccessor(0);
                                builder.CreateBr (block_taken);
                                
                                branIst->setSuccessor(0, block_next);
                                // Check Phi node 
                                Instruction *ins = block_taken->begin();
                                if (isa<PHINode>(ins)) {
                                    PHINode *PN = dyn_cast<PHINode>(ins);
                                    PN->setIncomingBlock(PN->getBasicBlockIndex(BB), block_next);
                                }

                                // Create a call to BranchTotal to record the total number of branches
                                builder.SetInsertPoint(branIst);
                                builder.CreateCall (BranchTotal, f_name_str);
                            }

                        }
                        // Check if return; Then print stats
                        if ((isa<ReturnInst>(BB_itr)) && (f_current->getName() == "main")) {
                            IRBuilder<> builder(BB);
                            BasicBlock::iterator last_block = BB->back();
                            builder.SetInsertPoint(last_block);
                            // Create a call to PrintStats
                            builder.CreateCall (PrintStats, "");                            
                        }
                    }
                }
            }
            return false;
        }
    };
}

char BranchBias::ID = 0;
static RegisterPass<BranchBias> X("branch_bias", "Branch Bias Profiling", false, false);
