#ifndef POINTER_ANALYSIS
#define POINTER_ANALYSIS
#include "WorkList.h"
#include <map>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <set>
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "PointerAnalysisFlow.h"


using namespace llvm;
using namespace std;



class PointerAnalysis : public WorkList {

public :

	PointerAnalysis(Function &F);
	Flow* executeFlowFunction(Flow *in, Instruction *inst, int NodeId);

	Flow* initialize();

    void print(raw_ostream &OS);
    void printHelper(raw_ostream &OS, LatticeNode* node);
	static const int X_rY = 1;
	static const int X_Y =2;
	static const int pX_Y =3;
	static const int X_pY =4;
	static const int KEEP_SAME=5;
  

	
	bool isPointer(Value * p);
	bool isVariable(Value * p);
	int whoAmI(PointerAnalysisFlow* in, Instruction* inst);
	
	//do operation task
	PointerAnalysisFlow* operation_X_rY(PointerAnalysisFlow* in, Instruction* inst);
	PointerAnalysisFlow* operation_X_Y(PointerAnalysisFlow* in, Instruction* inst);
	PointerAnalysisFlow* operation_pX_Y(PointerAnalysisFlow* in, Instruction* inst);
	PointerAnalysisFlow* operation_X_pY(PointerAnalysisFlow* in, Instruction* inst);
	
};
#endif
