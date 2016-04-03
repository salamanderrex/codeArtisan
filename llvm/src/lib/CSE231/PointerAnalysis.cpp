#include "PointerAnalysis.h"

Flow* PointerAnalysis::executeFlowFunction(Flow *in, Instruction *inst, int NodeId){
	PointerAnalysisFlow* inFlow = static_cast<PointerAnalysisFlow*>(in);
	PointerAnalysisFlow * output;
	errs()<<inst<<"\n";

	int opType = whoAmI(inFlow,inst);
	switch(opType) {
		case  X_rY:
				output = operation_X_rY(inFlow,inst);
				break;
		case  X_Y:
				output = operation_X_Y(inFlow,inst);
				break;
		case  pX_Y:
				output = operation_pX_Y(inFlow,inst);
				break;
		case  X_pY:
				output = operation_X_pY(inFlow,inst);
				break;
		default:
				output = new PointerAnalysisFlow(inFlow);

	}
	return output;
}

int PointerAnalysis::whoAmI(PointerAnalysisFlow * inFlow, Instruction * inst) {

	errs()<<inst<<"\n";
	int opcode = inst->getOpcode();
	if (opcode == 28)   {
		// 28 is sotre
		//store i32 5, i32* %y, align 4
		//which means x = &y  (NOTICE x=NULL is also in this range, y here is NULL)
		//store i32* %y, i32** %x1, align 8
		//for x ==NULL
		// store i32* null, i32** %a, align 8
		return  X_rY;
	}
	if (opcode ==29) 
	    //opcode 29 is Load 
		//If one load followed by one store, then is X = Y
		//If two loads followed by one store, then is *X = Y
		{
			Instruction* nInst = inst->getNextNode();
			Instruction* nnInst = nInst->getNextNode();
			if(isa<StoreInst>(nInst)) {
				return  X_Y;
			} else if (isa<LoadInst>(nInst) && isa<StoreInst>(nnInst)) {
				if (inst->getOperand(0)->getName()!="" && nInst->getOperand(0)->getName()!="")
					return  pX_Y;
				else if (inst->getOperand(0)->getName()!="" && nnInst->getOperand(1)->getName()!="")
					return  X_pY;			
	
		}
		}
		
	return  KEEP_SAME;


	
}




PointerAnalysisFlow* PointerAnalysis::operation_X_rY(PointerAnalysisFlow* in, Instruction* instruction) {
	errs()<<"Start x=&y analysis ================================="<<"\n";
	//Check that left operand is not null.
	
	if (isa<ConstantPointerNull>(instruction->getOperand(0))) {
		errs()<<"Null Pointer!!!"<<"\n";
		PointerAnalysisFlow* f = new PointerAnalysisFlow(in);
		Value* X = instruction->getOperand(1);
		if (isPointer(X) && isVariable(X)) {
			errs()<<"Remove  " <<X->getName()<<" from list"<<"\n";
			f->value.erase(X->getName());
		}
		
		//very important if value is empty, then it is a bottom
		if(f->value.size()==0) {
			f->triPoint=BOTTOM;
		}
		return f;
		//return execute_X_equals_NULL(in,instruction);
	}
	errs()<<"Not Null Pointer, move on"<<"\n";
	StoreInst* store = static_cast<StoreInst*>(instruction);
	PointerAnalysisFlow* f = new PointerAnalysisFlow(in);
	
	// X = &Y
	//Check if right  is a pointer
	if (store->getOperand(1)->getType()->isPointerTy()) {
		//Check if x y  names are variable
		if (store->getOperand(0)->getName()!="" && store->getOperand(1)->getName()!="") {

			PointerAnalysisFlow* ff = new PointerAnalysisFlow();
			set<string> s;
			map<string, set<string> >value;
			s.insert(store->getOperand(0)->getName());
			value[store->getOperand(1)->getName()] = s;
			// X now points to Y.
			ff->value = value;
			PointerAnalysisFlow* tmp = static_cast<PointerAnalysisFlow*>(ff->join(f));
			delete ff;
			delete f;
			f = tmp;
		}
	}
	return f;
}


PointerAnalysisFlow* PointerAnalysis::operation_X_Y(PointerAnalysisFlow* in, Instruction* instruction) {
	LoadInst* load = static_cast<LoadInst*>(instruction);
	PointerAnalysisFlow* f = new PointerAnalysisFlow(in);
	Value* Y = load->getOperand(0); //RO
	Value* X = load->getNextNode()->getOperand(1); //LO

	// X = Y
	// Check if X  Y are pointers.
	if (isPointer(Y) && isPointer(X)) {
		if (isVariable(Y) && isVariable(X)) {

			//x points to what y points to 
			PointerAnalysisFlow* ff = new PointerAnalysisFlow();
			map<string, set<string> > value;
			value[X->getName()] = in->value[Y->getName()];;
			ff->value = value;
			PointerAnalysisFlow* tmp = static_cast<PointerAnalysisFlow*>(ff->join(f));
			delete ff;
			delete f;
			f = tmp;
		}
	}
	return f;
}

PointerAnalysisFlow* PointerAnalysis::operation_pX_Y(PointerAnalysisFlow* in, Instruction* instruction){
	PointerAnalysisFlow* f = new PointerAnalysisFlow(in);
	Value* Y = instruction->getOperand(0); //RO
	Value* X = instruction->getNextNode()->getOperand(0); //LO
	//Check that both operands are pointers.
	if (Y->getType()->isPointerTy() && X->getType()->isPointerTy()) {
		if (Y->getName()!="" && X->getName()!="") {
			//Everything Y points to, *X points to now as well.
			//*X points to Y points to 
			PointerAnalysisFlow* ff = new PointerAnalysisFlow();
			set<string> pointedByX = in->value[X->getName()];
			map<string, set<string> > value;
			for (set<string>::iterator it = pointedByX.begin() ; it != pointedByX.end() ; it++) {

				string x = *it;
				value[x] = in->value[Y->getName()];
			}
			ff->value = value;
			PointerAnalysisFlow* tmp = static_cast<PointerAnalysisFlow*>(ff->join(f));
			delete ff;
			delete f;
			f = tmp;
		}
	}
	return f;
}


PointerAnalysisFlow* PointerAnalysis::operation_X_pY(PointerAnalysisFlow* in, Instruction* instruction) {
	PointerAnalysisFlow* f = new PointerAnalysisFlow(in);
	Value* Y = instruction->getOperand(0);
	Value* X = instruction->getNextNode()->getNextNode()->getOperand(1);
	if (Y->getType()->isPointerTy() && X->getType()->isPointerTy()) {
		if (Y->getName()!="" && X->getName()!="") {
			PointerAnalysisFlow* ff = new PointerAnalysisFlow();
			//For all W pointed by Y, X points to Z, where Z is pointed by W.
			set<string> pointedByY = in->value[Y->getName()]; //Set of Ws.
			for (set<string>::iterator W = pointedByY.begin(); W != pointedByY.end() ; W++) {
				for (set<string>::iterator Z = in->value[*W].begin() ; Z != in->value[*W].end() ; Z++) {
					ff->value[X->getName()].insert(*Z);
				}
			}
			PointerAnalysisFlow* tmp = static_cast<PointerAnalysisFlow*>(ff->join(f));
			delete ff;
			delete f;
			f = tmp;
		}
	}
	return f;
}


void PointerAnalysis::print(raw_ostream &OS) {

    for (unsigned int i = 0; i < CFGNodes.size() ; i++) {
        this->printHelper(OS,this->CFGNodes[i]);
        if(i+1 < CFGNodes.size()) {
            OS << "\n";
        }
            OS << "\n";
    }
}


void PointerAnalysis::printHelper(raw_ostream &OS, LatticeNode* node) {
    
	OS << "representation : " << *(node->inst) << "\n";
	OS << "#Edge incoming" << "\n";
    for (unsigned int i = 0 ; i < node->incoming.size() ; i++) {
 
		PointerAnalysisFlow * temp = (PointerAnalysisFlow * )node->incoming[i]->flow;
		OS << temp->arrowList() << "\n";

    }
	OS << "\n";
	
 	OS << "#Edge outcoming" << "\n";
    for (unsigned int i = 0 ; i < node->outgoing.size() ; i++) {

		PointerAnalysisFlow * temp = (PointerAnalysisFlow * )node->outgoing[i]->flow;
		OS<<temp->arrowList()<<"\n";
 
    }


}

bool PointerAnalysis::isPointer(Value * p) {
	return p->getType()->isPointerTy();
}
bool PointerAnalysis::isVariable(Value * X) {
	return X->getName() != "";
}

Flow* PointerAnalysis::initialize(){
	return new PointerAnalysisFlow(BOTTOM);
}

PointerAnalysis::PointerAnalysis(Function &F){

	this->functionName = F.getName();
    CFGmaker(F);
}
