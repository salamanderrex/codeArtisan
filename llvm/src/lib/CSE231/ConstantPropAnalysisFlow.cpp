#include "ConstantPropAnalysisFlow.h"


	
ConstantPropAnalysisFlow::ConstantPropAnalysisFlow(): Flow(){}
	
ConstantPropAnalysisFlow::ConstantPropAnalysisFlow(int triPoint): Flow(triPoint){}
	
ConstantPropAnalysisFlow::ConstantPropAnalysisFlow(ConstantPropAnalysisFlow *flow): Flow(flow->triPoint) {
	this->value = flow->value;
}

ConstantPropAnalysisFlow::~ConstantPropAnalysisFlow(){}	
	
bool ConstantPropAnalysisFlow::equals(Flow* otherSuper) {
	ConstantPropAnalysisFlow* other =
			static_cast<ConstantPropAnalysisFlow*>(otherSuper);
	// if one of them is "BOTTOM" or TOP	
	if (this->triPoint || other->triPoint)
		return this->triPoint == other->triPoint;
	//compare the size	
	if (other->value.size() != this->value.size())
		return false;
		
	for (map<string, float>::const_iterator it = this->value.begin(); it != this->value.end(); it++) {
		string key = it->first;
		float thisVal = it->second;
		if (other->value.find(key) == other->value.end())
			return false;
		float otherVal = other->value.find(key)->second;
		if (otherVal != thisVal)
			return false;

	}

	return true;
}

string ConstantPropAnalysisFlow::arrowList() {
	if (value.size()==0)
		return std::string ( ( this->triPoint ==BOTTOM ? "BOTTOM" : "TOP"));
	//Value has something inside
	stringstream ss;
	//ss<<"in arrowList";
	map<string, float >::const_iterator it = this->value.begin();
    string first = it->first;
	float second =it->second;
	
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


void ConstantPropAnalysisFlow::copy(Flow* rhs) {
	ConstantPropAnalysisFlow* f = static_cast<ConstantPropAnalysisFlow*>(rhs);
	this->triPoint = f->triPoint;
	this->value = f->value;
}
// TOP is empty set   BOTTOM is full set
Flow* ConstantPropAnalysisFlow::join(Flow* otherSuper) {

	ConstantPropAnalysisFlow* other =
			static_cast<ConstantPropAnalysisFlow*>(otherSuper);

	if (this->triPoint == TOP || other->triPoint == TOP)
		return new ConstantPropAnalysisFlow(TOP);

	if (this->triPoint == BOTTOM && other->triPoint == BOTTOM)
		return new ConstantPropAnalysisFlow(BOTTOM);

	if (this->triPoint == BOTTOM) {
		ConstantPropAnalysisFlow* f = new ConstantPropAnalysisFlow();
		//f->value = other->value;
		//f->triPoint = other->triPoint;
		f->copy(other);
		return f;
	}
	if (other->triPoint == BOTTOM) {
		ConstantPropAnalysisFlow* f = new ConstantPropAnalysisFlow();
	//	f->value = this->value;
	//	f->triPoint = this->triPoint;
		f->copy(this);
		return f;
	}

	ConstantPropAnalysisFlow* f = new ConstantPropAnalysisFlow();
  // if the two set are both not BOTTOM or TOP, join them
	for (map<string, float>::iterator it = this->value.begin();
			it != this->value.end(); it++) {

		if (other->value.find(it->first) == other->value.end()) {

			f->value[it->first] = this->value.find(it->first)->second;
		} 
	else {
		float otherValue = other->value.find(it->first)->second;
		float thisValue = this->value.find(it->first)->second;

			if (otherValue == thisValue)

				f->value[it->first] = otherValue;

		}
	}

	for (map<string, float>::iterator it = other->value.begin();
			it != other->value.end(); it++) {

		if (this->value.find(it->first) == this->value.end()) {
			f->value[it->first] = other->value.find(it->first)->second;
		}
		else {
			float thisValue = this->value.find(it->first)->second;
			float otherValue = other->value.find(it->first)->second;

			if (otherValue == thisValue)
				f->value[it->first] = otherValue;
		}
	}
	return f;

}
