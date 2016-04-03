#include "PointerAnalysisFlow.h"

bool PointerAnalysisFlow::equals(Flow* otherSuper) {
	PointerAnalysisFlow* other = static_cast<PointerAnalysisFlow*>(otherSuper);
	
	//print our for test 
	errs()<<"************Start ***********"<<"\n";
	errs()<<"this->triPoint"<<this->triPoint<<" other->TriPoint"<<other->triPoint<<"\n";
	if (this->triPoint || other->triPoint) {
		errs()<<"************end for TOP or BOTTOM TYPE ***********"<<"\n";
		return this->triPoint == other->triPoint;
	}
		
		
	errs()<<"check this value"<<"\n:";
	printValue(this->value);
	errs()<<"\ncheck other value"<<"\n";
	printValue(other->value);
	errs()<<"************end ***********"<<"\n";
	if (other->value.size()!=this->value.size())
			return false;
	errs()<<"size is same"<<this->value.size()<<"\n";
	
	for (map<string, set<string> >::const_iterator it = this->value.begin(); it != this->value.end() ; it++) {
		string key = it->first;
		set<string> thisSet = it->second;
		
		
		
		if(!other->value.count(key))
			return false;
		set<string> otherSet = other->value.find(key)->second;
		 for (set<string>::iterator it=thisSet.begin(); it!=thisSet.end(); ++it) {
			 if(otherSet.find(*it)==otherSet.end()){
				 return false;
			 }
		 }
	}
	return true;
}



void PointerAnalysisFlow::copy(Flow* rhs) {
	PointerAnalysisFlow* f = static_cast<PointerAnalysisFlow*>(rhs);
	this->triPoint = f->triPoint;
	this->value = f->value;
}

Flow* PointerAnalysisFlow::join(Flow* otherInput) {
	//join bottom-bottom gives you bottom. Anything else gives you top.
	
	
	PointerAnalysisFlow* other = static_cast<PointerAnalysisFlow*>(otherInput);

	//BOTTOM + BOTTOM = BOTTOM
	if (this->triPoint == BOTTOM && other->triPoint == BOTTOM)
		return new PointerAnalysisFlow(triPoint);

	//BOTTOM + VALUE = VALUE
	if (this->triPoint == BOTTOM) {
		PointerAnalysisFlow* f = new PointerAnalysisFlow();
		f->copy(other);
		return f;
	}
	if (other->triPoint == BOTTOM) {
		PointerAnalysisFlow* f = new PointerAnalysisFlow();
		f->copy(this);
		return f;
	}

	//TOP + TOP == TOP
	if (this->triPoint == TOP || other->triPoint == TOP)
		return new PointerAnalysisFlow(TOP);



	//Value + value += NEW VALUE
	PointerAnalysisFlow* f = new PointerAnalysisFlow();
	
	//For easy, here Anderson
	set<string> keys;
	for (map<string, set<string> >::iterator it = this->value.begin() ; it != this->value.end() ; it++)
		keys.insert(it->first);
	for (map<string, set<string> >::iterator it = other->value.begin() ; it != other->value.end() ; it++)
		keys.insert(it->first);
	for (set<string>::iterator it = keys.begin() ; it != keys.end() ; it++) {
		string key = *it;
		set<string> values;
		for (set<string>::iterator j = this->value[key].begin(); j != this->value[key].end() ; j++)
			values.insert(*j);
		for (set<string>::iterator j = other->value[key].begin(); j != other->value[key].end() ; j++)
			values.insert(*j);
		if (values.size()>0)
			//insert into the new f
			f->value[key] = values;
	}
	return f;

}


string PointerAnalysisFlow::arrowList() {
	if (value.size()==0)
		return std::string ( ( this->triPoint ==BOTTOM ? "BOTTOM" : "TOP"));
	//Value has something inside
	stringstream ss;
	//ss<<"in arrowList";
	map<string, set<string> >::const_iterator it = this->value.begin();
    string first = it->first;
	set<string>::iterator its=it->second.begin();
	if (its != it->second.end()) {
        ss << first << "->"<< *its << "\n";
        its++;
	}
	for (; its != it->second.end() ; its++) {
        ss << first << "->"<< *its << "\n";
	}
	//errs() << "number of keys in set : " << it->second.size() << "\n";
 	if (it != this->value.end())
 		it++;
	for (; it != this->value.end() ; it++) {
		if (it->second.size()==0)
			continue;
        string first = it->first;
		its=it->second.begin();
		if (its != it->second.end()) {
            ss << first << "->"<< *its << "\n";
			its++;
		}
		for (; its != it->second.end() ; its++) {
            ss << first << "->"<< *its << "\n";
		}
	}
	ss << "";
	//ss<<"out arrowList";
	return ss.str();
}


void PointerAnalysisFlow::printValue(map<string, set<string> > value) {
	
	errs()<<"value size is "<< value.size()<<"\n";
	if (value.size()) {
	
		for (map<string, set<string> >::const_iterator it = value.begin(); it != value.end() ; it++) {
		string key = it->first;
		errs() << "key: " << key<<"\n" ;
		set<string> thisSet = it->second;
		errs()<< "set size : " << thisSet.size()<<" [";
		for (set<string>::iterator it=thisSet.begin(); it!=thisSet.end(); ++it) {
			errs()<< key<< " --->" << *it <<" ";
		}
		errs()<<"]\n";
		
		}
	}
	
}

PointerAnalysisFlow::PointerAnalysisFlow() {
	this->triPoint = 0;
}

PointerAnalysisFlow::PointerAnalysisFlow(int triPoint)  {
	this->triPoint = triPoint;
}

PointerAnalysisFlow::PointerAnalysisFlow(PointerAnalysisFlow *flow) {
	this->triPoint = flow->triPoint;
	this->value = flow->value;
	
}


PointerAnalysisFlow::~PointerAnalysisFlow() {
	
}
