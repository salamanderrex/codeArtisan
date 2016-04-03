
#ifndef FLOW_H_
#define FLOW_H_


#include "llvm/Support/raw_ostream.h"


#include <string>


using namespace std;
using namespace llvm;


class Flow {

public :
    // LatticeBase * base;
    //TriState is the world of lattice
    //Bottom =1 , TOP 2 or NOT Bottom and TOP 0
    int triPoint;

	virtual bool equals(Flow* other);

	//copy Flow
	virtual void copy(Flow *rhs);
    //join flow
	virtual Flow* join(Flow* other);

	Flow();

    Flow(int triPoint);
	Flow(Flow* flow);

	virtual ~Flow();


};


//old LatticeBase, does not use any more, use triPOint instead
/*
class LatticeBase {
public:
     static const std::string TOP;
     static const  std::string BOTTOM;

    string value;

    LatticeBase() {
        this->value = "";
    }
    LatticeBase(std::string input) {
        this->value = input; 

    } 
    //valid is isBasic
    bool valid() {
        return  this->value != NULL;
    }
    
    bool equals(LatticeBase * other) {
        return this->value==other->value;
    }
    void copy(LatticeBase * copyee) {
        this->value  = copyee->value;
        
    }

};
*/

#endif
