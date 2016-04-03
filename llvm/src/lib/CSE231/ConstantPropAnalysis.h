

#ifndef CONSTANT_PROP_ANALYSIS
#define CONSTANT_PROP_ANALYSIS
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "ConstantPropAnalysisFlow.h"
#include "WorkList.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <vector>
#include <cstdlib>
#include <queue>
#include <sstream>
#include <set>
#include <llvm/IR/Constants.h>

using namespace llvm;
using namespace std;

//Static Analysis class
class ConstantPropAnalysis : public WorkList {

public :

	ConstantPropAnalysis(Function &F);

	Flow* executeFlowFunction(Flow *in, Instruction *inst, int NodeId);

	Flow* initialize();
	
	void print(raw_ostream &OS);
	
	void printHelper(raw_ostream &OS, LatticeNode* node);
	

	ConstantPropAnalysisFlow *runCastInst(ConstantPropAnalysisFlow* in, Instruction* inst);

	ConstantPropAnalysisFlow *returnTop();

	ConstantPropAnalysisFlow *runFOpInst(ConstantPropAnalysisFlow* in, Instruction* inst, unsigned opcode);
	ConstantPropAnalysisFlow *runOpInst(ConstantPropAnalysisFlow* in, Instruction* inst, unsigned opcode);
	ConstantPropAnalysisFlow *runPhiInst(ConstantPropAnalysisFlow* in, Instruction* inst);



	float computeOp(float leftVal, float rightVal, unsigned opcode);





};
#endif
