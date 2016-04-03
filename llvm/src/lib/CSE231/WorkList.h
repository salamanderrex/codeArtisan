#ifndef WORKLIST
#define WORKLIST
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "Lattice.h"
#include "Flow.h"
#include <map>
#include <vector>
#include <cstdlib>
#include <queue>
#include <sstream>
#include <set>

using namespace llvm;
using namespace std;


class WorkList {

public :

	
	void runWorklist();

    virtual void print(raw_ostream &OS);
	
	WorkList(Function &F);
	WorkList(); 
	virtual ~WorkList();

	virtual Flow* executeFlowFunction(Flow *in, Instruction *inst, int NodeId);


	virtual Flow* initialize();
	void CFGmaker(Function &F);

	LatticeNode* root;
	vector<LatticeNode*> CFGNodes;
	vector<LatticeEdge*> CFGEdges;
	StringRef functionName;
};
#endif
