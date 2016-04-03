#ifndef CSE_ANALYSIS
#define CSE_ANALYSIS
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "CSEAnalysisFlow.h"
#include "WorkList.h"
#include <map>
#include <vector>
#include <cstdlib>
#include <queue>
#include <sstream>
#include <set>
#include <llvm/IR/Constants.h>
#include <time.h>
#include <stdlib.h>

using namespace llvm;
using namespace std;

class CSEAnalysis: public WorkList {

public:

	CSEAnalysis(Function &F);

	Flow* executeFlowFunction(Flow *in, Instruction *inst, int NodeId);

	Flow* initialize();
void print(raw_ostream &OS);
	
	void printHelper(raw_ostream &OS, LatticeNode* node);

protected:

	CSEAnalysisFlow *runUnaryInst(
			CSEAnalysisFlow* in, Instruction* inst,
			unsigned opcode);
	CSEAnalysisFlow *runFOpInst(
			CSEAnalysisFlow* in, Instruction* inst,
			unsigned opcode);
	CSEAnalysisFlow *runOpInst(
			CSEAnalysisFlow* in, Instruction* inst,
			unsigned opcode);
	CSEAnalysisFlow *runPhiInst(
			CSEAnalysisFlow* in, Instruction* inst);

private:
	string computeBinaryOp(string leftVal, string rightVal, unsigned opcode);
	string computeUnaryOp(string leftVal, unsigned opcode);
	bool isEqual(CSEAnalysisFlow* in, Instruction *inst);
	map<string, string> analysisMap;

};
#endif
