#include "CSEAnalysisFlow.h"

#define TOP 2
#define BOTTOM 1

CSEAnalysisFlow::CSEAnalysisFlow() :
		Flow() {}

CSEAnalysisFlow::CSEAnalysisFlow(int triPoint) :
		Flow(triPoint) {}

CSEAnalysisFlow::CSEAnalysisFlow(
		CSEAnalysisFlow *flow) :
		Flow(flow->triPoint) {this->value = flow->value;}

bool CSEAnalysisFlow::equals(Flow* otherSuper) {
	CSEAnalysisFlow* other =
			static_cast<CSEAnalysisFlow*>(otherSuper);
	if (this->triPoint || other->triPoint)
		return this->triPoint == other->triPoint;
	if (other->value.size() != this->value.size())
		return false;
	for (map<string, string>::const_iterator it = this->value.begin();
			it != this->value.end(); it++) {
		string key = it->first;
		string thisVal = it->second;
		//Check if key is found in other
		if (other->value.find(key) == other->value.end())
			return false;
		string otherVal = other->value.find(key)->second;
		if (otherVal != thisVal)
			return false;

	}

	return true;
}
string CSEAnalysisFlow::arrowList() {
	if (value.size()==0)
		return std::string ( ( this->triPoint ==BOTTOM ? "BOTTOM" : "TOP"));
	//Value has something inside
	stringstream ss;
	//ss<<"in arrowList";
	map<string, string >::const_iterator it = this->value.begin();
    string first = it->first;
	string second =it->second;
	
        ss << first << "->"<< second<< "\n";
  
	//errs() << "number of keys in set : " << it->second.size() << "\n";
 	if (it != this->value.end())
 		it++;
	for (; it != this->value.end() ; it++) {
        string first = it->first;
		second = it->second;
        ss << first << "->"<< second << "\n";
	}
	ss << "";
	//ss<<"out arrowList";
	return ss.str();
}

void CSEAnalysisFlow::copy(Flow* rhs) {
	CSEAnalysisFlow* f = static_cast<CSEAnalysisFlow*>(rhs);
	this->triPoint = f->triPoint;
	this->value = f->value;
}
Flow* CSEAnalysisFlow::join(Flow* otherSuper) {

	CSEAnalysisFlow* other =
			static_cast<CSEAnalysisFlow*>(otherSuper);

	if (this->triPoint == BOTTOM && other->triPoint == BOTTOM)
		return new CSEAnalysisFlow(BOTTOM);

	if (this->triPoint == BOTTOM) {
		CSEAnalysisFlow* f = new CSEAnalysisFlow();
                f->triPoint = other->triPoint;
	        f->value = other->value;
		return f;
	}
	if (other->triPoint == BOTTOM) {
		CSEAnalysisFlow* f = new CSEAnalysisFlow();
		f->triPoint = this->triPoint;
	        f->value = this->value;
		return f;
	}

	if (this->triPoint == TOP || other->triPoint == TOP)
		return new CSEAnalysisFlow(TOP);

	CSEAnalysisFlow* f = new CSEAnalysisFlow();

	for (map<string, string>::iterator it = this->value.begin();
			it != this->value.end(); it++) {

		if (other->value.find(it->first) == other->value.end()) {
			f->value[it->first] = this->value.find(it->first)->second;
		} else {
			string otherVal = other->value.find(it->first)->second;
			string thisVal = this->value.find(it->first)->second;

			if (otherVal == thisVal)
				f->value[it->first] = otherVal;

		}
	}

	for (map<string, string>::iterator it = other->value.begin();
			it != other->value.end(); it++) {

		if (this->value.find(it->first) == this->value.end()) {
			f->value[it->first] = other->value.find(it->first)->second;
		} else {
			string thisVal = this->value.find(it->first)->second;
			string otherVal = other->value.find(it->first)->second;

			if (otherVal == thisVal)

				f->value[it->first] = otherVal;

		}
	}

	return f;

}

CSEAnalysisFlow::~CSEAnalysisFlow() {
}
