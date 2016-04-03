#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "WorkList.h"
#include "RAFlow.h"
#include <map>

using namespace llvm;
using namespace std;


typedef struct _node
{
	int counter;
	RAFlow* nodeSet;
	_node(){counter = 0;};
}node;

//get it from interface
class RangeAnalysis : public WorkList{

	public:	
		RangeAnalysis(Function &F);
	 	Flow* executeFlowFunction(Flow *in, Instruction *inst, int NodeId);
	 	Flow* initialize();
	 	void print(raw_ostream &OS);
	 	void printHelper(raw_ostream &OS, LatticeNode* node);

	protected:
		RAFlow *runCastInst(RAFlow * in, Instruction* inst);
		RAFlow *runOpInst(RAFlow * in, Instruction* inst, unsigned opcode, int type);
		RAFlow *runPhiInst(RAFlow * in, Instruction* inst);
		RAFlow *runStoreInst(RAFlow * in, Instruction* inst);
		RAFlow *runLoadInst(RAFlow * in, Instruction* inst);
		RAFlow *runCompInst(RAFlow* in, Instruction* instruction);
		RAFlow *runRtnInst(RAFlow* in, Instruction* instruction);

		map<int,node> nodeCount;

	public:
		Range getRange(Range leftRange, Range rightRange, unsigned opcode);

};
