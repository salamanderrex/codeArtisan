#ifndef POINTER_ANALYSIS_FLOW
#define POINTER_ANALYSIS_FLOW
#include <string>
#include <map>
#include <set>
#include <sstream>
#include "llvm/Support/raw_ostream.h"
#include "Flow.h"
#include "Lattice.h"

using namespace std;
using namespace llvm;


class PointerAnalysisFlow: public Flow {

public:

	
	bool equals(Flow* other);

    string arrowList();

	void copy(Flow* rhs);

	Flow* join(Flow* other);

	PointerAnalysisFlow();

	PointerAnalysisFlow(int triPoint);

	PointerAnalysisFlow(PointerAnalysisFlow* flow);

	~PointerAnalysisFlow();

	map<string, set<string> > value;
	//for test to print value
	void printValue(map<string, set<string> > value);
};

#endif
