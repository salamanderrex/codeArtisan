#include "Flow.h"
#include "Lattice.h"
bool Flow::equals(Flow* other){
	return this->triPoint == other->triPoint;
}

void Flow::copy(Flow *rhs){
	 this->triPoint = rhs->triPoint;
}


Flow::Flow(){

    triPoint=0;
}


Flow::Flow(int triPoint) {
	this->triPoint = triPoint;
}
Flow::Flow(Flow* flow){

    triPoint = flow->triPoint;
}

Flow* Flow::join(Flow* other){

	//BOTTOM + BOTTOM = BOTTOM
	//ElSE return TOP
	errs()<< "you shall not pass \n";
	if (this->triPoint==BOTTOM && other->triPoint==BOTTOM)
		return new Flow(BOTTOM);
	else
		return new Flow(TOP);
	
}

Flow::~Flow(){

   
}
