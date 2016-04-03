// CSE 231 Project Part 1
// Collecting Static Instruction Counts
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include "llvm/Support/InstIterator.h"
//for iterating over BasicBlock's Instructions, InstIterator should be used
#include "llvm/IR/Value.h"
#include <map>

using namespace llvm;
using namespace std;

namespace {
  
  struct CountStaticInstructions : public ModulePass {
    static char ID;

    std::map<string, int> InstructionCount; // maps for the number of instructions
    
    CountStaticInstructions() : ModulePass(ID) {}
    
    virtual bool runOnModule(Module &M){
      for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI){
        // function list iterator
        for (inst_iterator I = inst_begin(MI), E = inst_end(MI); I != E; ++I){
          // instructions iterator	  
            string  inst_name = Instruction::getOpcodeName(I->getOpcode());
           // getOpcode() returns the opcode for the Instruction
          if (InstructionCount[inst_name] > 0){
            InstructionCount[inst_name] = InstructionCount[inst_name] + 1;
          }
          else{
            InstructionCount[inst_name] = 1;
          }
        }
      }

      return false;
    }
    
    // Override print method to output.
    virtual void print(llvm::raw_ostream &Output, const Module *M) const {
      // We loop over elements of the map and print out the number of times they occured.
      // Because of various inheritance issues, it is a const_iterator.
      int total_num = 0;
      for (std::map<string, int>::const_iterator mit = InstructionCount.begin(); mit!=InstructionCount.end(); ++mit){
        Output << mit->first << ": " << mit->second << "\n";

        total_num += mit->second;
     } 
      Output << "TOTAL:   " << total_num << "\n";
    }

  };
}
char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("CountStaticInstructions", "CountStaticInstructions Pass", false, false);
// give our class CountStaticInstructions a commang line argument "CountStaticInstruction" and a name "CountStaticInstruction pass"
