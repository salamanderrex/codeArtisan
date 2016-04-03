#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include <string>
#include <map>

using namespace llvm;
using namespace std;


namespace {
    struct DYCount : public ModulePass {
        static char ID;


        DYCount() : ModulePass(ID) {}

        virtual bool runOnModule(Module &M){

            //function in dynamic helper 
            //Count(char * opName, int count)
            //prepare the arguments
            std::vector <Type*> argsForCount;
            argsForCount.push_back(Type::getInt8PtrTy(M.getContext()));
            argsForCount.push_back(IntegerType::get(M.getContext(),32));

            //make the fucntion type
            FunctionType * functiontype = FunctionType::get (
                    Type::getVoidTy(M.getContext()) ,
                    argsForCount,
                    false
                    );

            //create function
            Function * Count = Function::Create(functiontype,Function::ExternalLinkage,"_Z5CountPci",&M);

            //function in dynamic_helper
            //printDI ()  
            std::vector <Type*> argsForPrint;
            FunctionType * functiontype_print = FunctionType::get (
                    Type::getVoidTy(M.getContext()),
                    argsForPrint,
                    false
                    );

            //create fucntion print
            Function *Print = Function::Create ( functiontype_print,
                    Function::ExternalLinkage, 
                    "_Z7printDIv",
                    &M
                    );

            //iterate over module -> function -> bb -> instruction
            for (Module::iterator M_iterator = M.begin(), M_iterator_end = M.end(); M_iterator != M_iterator_end; M_iterator++){
                for(Function::iterator it_BB = M_iterator->begin(), End = M_iterator->end(); it_BB != End; ++it_BB)
                {
                    map<string, int> InstCountMap;
                    Instruction* inst;
                    //count the instruction in function blocks
                    for (BasicBlock::iterator it_Inst = it_BB->begin(), it_Inst_end = it_BB->end(); it_Inst != it_Inst_end; it_Inst++){
                        inst = &*it_Inst;
                        string opname = inst->getOpcodeName();
                        if (InstCountMap.count(opname) == 0){
                            InstCountMap[opname] = 1;
                        }
                        else {
                            InstCountMap[opname]++;
                        }
                    }

                    IRBuilder<> builder(it_BB);
                    BasicBlock::iterator bbIter = it_BB->end();
                    bbIter--;
                    builder.SetInsertPoint(bbIter);

                    map <string, int>::const_iterator iter;
                    for (iter = InstCountMap.begin(); iter != InstCountMap.end(); iter++){
                        //user pointer to point the opname string
                        Value *  myopname = builder.CreateGlobalStringPtr(iter->first);
                        Value *  count_for_this_BB = ConstantInt::get(Type::getInt32Ty(M.getContext()), iter->second);

                        builder.CreateCall2(Count, myopname, count_for_this_BB);
                    }

                }

                // Add the print helper function before the program's end
                // i.e. @the end of main
                if ((*M_iterator).getName() =="main" ) {
                    Function::iterator lastBlock = M_iterator->end();
                    // find last block in main
                    IRBuilder<> builder(--lastBlock);
                    //find last inst in last block
                    BasicBlock::iterator lastIns =lastBlock->end();
                    builder.SetInsertPoint(--lastIns);

                    //should keep "", cannot use NULL pointer
                    builder.CreateCall (Print, "");

                }
            }
            return false;
        }


    };
}

char DYCount::ID = 0;
static RegisterPass<DYCount> X("dynamic_count", "Dynamic Instruction Count", false, false);
