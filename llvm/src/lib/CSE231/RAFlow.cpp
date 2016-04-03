#include "RAFlow.h"
#include <map>



Range JoinRange(const Range* left, const Range* right)
{
	Range maxRange;
	Range maxAB;

	if((left->bottom == true) && (right->bottom == true)) return maxRange;
	if(left->bottom == true) return *right;
	if(right->bottom == true) return *left;

	if(left->lower < right->lower)
		maxRange.lower = left->lower;
	else
		maxRange.lower = right->lower;
	if(left->upper > right->upper)
		maxRange.upper = left->upper;
	else
		maxRange.upper = right->upper;
	maxRange.top = (left->top | right->top);
	maxRange.bottom = (left->bottom & right->bottom);

	return maxRange;
}

bool RangeEqual(const Range* left, const Range* right)
{
	if((left->lower == right->lower) & (left->upper == right->upper)) 
		return true;
	else
		return false;
}

bool RAFlow::equals(Flow* input)
{
	RAFlow* inputf = static_cast<RAFlow*>(input);
	if (this->triPoint!=0 || inputf->triPoint!=0)
		return this->triPoint == inputf->triPoint;
	if (inputf->value.size() != this->value.size())
		return false;
	for (map<string, Range>::const_iterator itr = this->value.begin(); itr != this->value.end(); itr++) {
		string key = itr->first;
		Range thisRange = itr->second;
		if (inputf->value.find(key) == inputf->value.end())
			return false;
		Range inputRange = inputf->value.find(key)->second;
		if(!RangeEqual(	(const Range*) &inputRange,(const Range*) &thisRange)	)
			return false;

	}
	return true;
}

void RAFlow::copy(Flow* rhs) {
	RAFlow* f = static_cast<RAFlow*>(rhs);
	this->triPoint = f->triPoint;
	this->value = f->value;
}

RAFlow::RAFlow() :
		Flow() {
}

RAFlow::RAFlow(int triPoint) :
		Flow(triPoint) {
}

RAFlow::RAFlow(RAFlow *flow) :
		Flow(flow->triPoint) {
	this->triPoint = flow->triPoint;
	this->value = flow->value;
}

Flow* RAFlow::join(Flow* input) {
	RAFlow* inputf = static_cast<RAFlow*>(input);
	if (this->triPoint == 1 && inputf->triPoint == 1)
		return new RAFlow(this->triPoint);
	if (this->triPoint == 1 ) {
		RAFlow* f = new RAFlow(0);
		f->copy(inputf);
		return f;
	}
	if (inputf->triPoint == 1) {
		RAFlow* f = new RAFlow(0);
		f->copy(this);
		return f;
	}
	if (this->triPoint == 2 || inputf->triPoint == 2)
		return new RAFlow(2);

	RAFlow* f = new RAFlow(inputf);
	//errs() << inputf->value.size() << "\n";
	for (map<string, Range>::iterator it = this->value.begin(); it != this->value.end(); it++) {
		if (f->value.find(it->first) == f->value.end()) {
			f->value[it->first] = it->second;
		} 
		else {
			Range inputRange = inputf->value.find(it->first)->second;
			Range thisRange = this->value.find(it->first)->second;
			Range joinedRange = JoinRange((const Range*) &inputRange,(const Range*) &thisRange);
			f->value[it->first] = joinedRange;
			//errs() << inputRange.lower << "," << inputRange.upper << "; " << thisRange.lower << "," << thisRange.upper << "; " << joinedRange.lower << ", " << joinedRange.upper << "\n";
		}
	}
	return f;

}

RAFlow::~RAFlow() {}

string RAFlow::toString() {
	stringstream ss;
	if (value.size() == 0)
		ss << " \"" << "value.size=" << value.size() << "\"";
	else{
		ss << "{";
		for (map<string, Range>::const_iterator itr = this->value.begin(); itr != this->value.end(); itr++) {
			ss << "\"" << itr->first << "\" : [";
			Range v = itr->second;
			ss << v.lower << "," << v.upper << "]; ";
		}
		ss << "}";
	}
	return  ss.str();
}
