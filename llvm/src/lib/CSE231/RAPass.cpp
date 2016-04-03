#include "llvm/Pass.h"
#include "RangeAnalysis.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

namespace {
  struct RAPass : public FunctionPass {
    static char ID;
    vector<RangeAnalysis*> RangeAnalyses;
    RAPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
    	RangeAnalyses.push_back(new RangeAnalysis(F));
    	return false;
    }
    void print(raw_ostream &OS, const Module*) const {
        for (unsigned int i = 0 ; i < RangeAnalyses.size() ; i++){
            OS << "Function Name:" << RangeAnalyses[i]->functionName << "\n";
            RangeAnalyses[i]->runWorklist();
            RangeAnalyses[i]->print(OS);
            OS << "\n";
        }
    }
  };
}

char RAPass::ID = 0;
static RegisterPass<RAPass> X("range_analysis", "Range Analysis Pass", false, false);
