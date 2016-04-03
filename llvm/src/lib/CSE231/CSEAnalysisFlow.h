#ifndef AVAILABLE_EXPRESSION_ANALYSIS_FLOW_H_
#define AVAILABLE_EXPRESSION_ANALYSIS_FLOW_H_
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include "llvm/Support/raw_ostream.h"
#include "Flow.h"

using namespace std;
using namespace llvm;
/*
 * This class is the FLow class for the constant propagation analysis.
 * This is a May-Point-To Analysis.
 */

class CSEAnalysisFlow: public Flow {

public:
        CSEAnalysisFlow();

	CSEAnalysisFlow(int triPoint);

	CSEAnalysisFlow(CSEAnalysisFlow* flow);

	~CSEAnalysisFlow();

	bool equals(Flow* other);

	Flow* join(Flow* other);

	map<string, string > value;
	
	void copy(Flow* rhs);
	string arrowList();


};

#endif 
