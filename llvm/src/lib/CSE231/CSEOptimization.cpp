#include "llvm/Pass.h"

#include "CSEAnalysis.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

namespace {
  struct CSEAnalysisGuardianPass : public FunctionPass {
    static char ID;
    vector<CSEAnalysis *>CSEAnalyses;
    CSEAnalysisGuardianPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
    	CSEAnalyses.push_back(new CSEAnalysis(F));
    	return false;
    }

    void print(raw_ostream &OS, const Module*) const {

    	OS << "Available Expression AnalysisS: \n";
    	for (unsigned int i = 0 ; i < CSEAnalyses.size() ; i++){
            CSEAnalyses[i]->runWorklist();
    		CSEAnalyses[i]->print(OS);
        
        	OS << "\n";
    	}

  	}
  };
}

char CSEAnalysisGuardianPass::ID = 0;
static RegisterPass<CSEAnalysisGuardianPass> X("CSEAnalysisGuardian", "Available Expression Analysis Guardian Pass", false, false);
