#ifndef CONSTANT_PROP_ANALYSIS_FLOW
#define CONSTANT_PROP_ANALYSIS_FLOW
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include "llvm/Support/raw_ostream.h"
#include "Flow.h"
#include "Lattice.h"

using namespace std;
using namespace llvm;
/*
 * This class is the FLow class for the constant propagation analysis.
  This is a Must-Point-To Analysis.
*/

class ConstantPropAnalysisFlow: public Flow {

public:
  
	map<string, float > value;
	
    string arrowList();

	//constructor 
	ConstantPropAnalysisFlow();

	ConstantPropAnalysisFlow(int triPoint);

	ConstantPropAnalysisFlow(ConstantPropAnalysisFlow* flow);

	~ConstantPropAnalysisFlow();
		
	bool equals(Flow* other);

	Flow* join(Flow* other);
	
	void copy(Flow* rhs) ;


};

#endif
