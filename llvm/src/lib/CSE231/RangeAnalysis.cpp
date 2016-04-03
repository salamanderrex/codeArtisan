#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include <string>
#include <limits>
#include <map>
#include "RangeAnalysis.h"
//#include "RAFlow.h"
#define BOTTOM 1
#define TOP 2

using namespace llvm;
using namespace std;

Flow* RangeAnalysis::executeFlowFunction(Flow *in, Instruction *inst, int NodeId) {
	int thr=3;
	RAFlow* inFlow = static_cast<RAFlow*>(in);
	RAFlow* output;
	int count;
	//add termination for loops
	if (nodeCount.find(NodeId) != nodeCount.end())
	{
		count = nodeCount[NodeId].counter;
		if (count >= thr)
		{
			RAFlow* f = new RAFlow(inFlow);
			RAFlow* fnew = new RAFlow();
			f = static_cast<RAFlow*>(fnew->join(f));
			RAFlow* tmp = nodeCount[NodeId].nodeSet;
			for (map<string, Range>::iterator itr = tmp->value.begin(); itr!=tmp->value.end(); itr++)
			{
				if((f->value.find(itr->first) != f->value.end())){
					Range fRange = f->value.find(itr->first)->second;
					Range nodeRange = tmp->value.find(itr->first)->second;
					if(!RangeEqual((const Range*) &fRange, (const Range*) &nodeRange)){
						fRange.lower = 0;
						fRange.upper = 0;
						fRange.top = true;
						fRange.bottom = false;
						fRange.undefined = true;
						tmp->value[itr->first] = fRange;
						f->value[itr->first] = fRange;
					}
				}
			}
			return f;
		}
		nodeCount[NodeId].counter = nodeCount[NodeId].counter + 1;
	} 
	else{
		RAFlow* f = new RAFlow(inFlow);
		RAFlow* fnew = new RAFlow();
		f = static_cast<RAFlow*>(fnew->join(f));

		// add this node
		node thisNode;
		thisNode.counter = 1;
		thisNode.nodeSet = f;
		nodeCount[NodeId] = thisNode;
	}

	switch (inst->getOpcode()) {
		case Instruction::ICmp:
		case Instruction::FCmp:
			output = runCompInst(inFlow, inst);
			break;
		case Instruction::Add:
		case Instruction::Sub:
		case Instruction::Mul:
		case Instruction::SDiv:
		case Instruction::SRem:
		case Instruction::Shl:
		case Instruction::AShr:
			output = runOpInst(inFlow, inst, inst->getOpcode(), 0);
			break;
		case Instruction::FAdd:
		case Instruction::FSub:
		case Instruction::FMul:
		case Instruction::FDiv:
		case Instruction::FRem:
			output = runOpInst(inFlow, inst, inst->getOpcode(), 1);
			break;
		case Instruction::PHI:
			output = runPhiInst(inFlow, inst);
			break;
		case Instruction::Store:
		    output = runStoreInst(inFlow, inst);
		    break;		
		case Instruction::Load:
		 	output = runLoadInst(inFlow, inst);
		    break;
		case Instruction::Trunc:
		case Instruction::ZExt:
		case Instruction::SExt:
		case Instruction::FPTrunc:
		case Instruction::FPExt:
		case Instruction::FPToUI:
		case Instruction::FPToSI:
		case Instruction::UIToFP:
		case Instruction::SIToFP:
			output = runCastInst(inFlow, inst);
			break;
		case Instruction::Ret:
			output = runRtnInst(inFlow, inst);
			break;
		default:
			output = new RAFlow(inFlow);
			break;
	}	
	//errs() << inst->getOperand(0)->getName()<<"\n";
	//errs() << *inst << output->toString() << "\n";
	return output;
}

// Note here we use "type" to distinguish int ("type=0") and float ("type=1")
Range getConstRange(Value* c, int type) {
	Range rtn; 
	if (type == 0)
	{
		ConstantInt *cI = dyn_cast<ConstantInt>(c);
		rtn.bottom = false;
		if(cI->isNegative()){
			// too large!
			if(cI->getBitWidth() > 32){
				rtn.upper = -std::numeric_limits<float>::infinity();
				rtn.lower = rtn.upper;
			}
			else{
				rtn.upper = (float) cI->getSExtValue();
				rtn.lower = rtn.upper;
			}
		}
		else{
			rtn.bottom = false;
			rtn.upper = (float) cI->getZExtValue();
			rtn.lower = rtn.upper;
		}
	}
	if (type == 1)
	{
		ConstantFP *cF = dyn_cast<ConstantFP>(c);
		rtn.bottom = false;
		rtn.upper = cF->getValueAPF().convertToFloat();
		rtn.lower = rtn.upper;
	}
	return rtn;
}

Range getConstRange(Value* c) {
	Range rtn;
	if (ConstantInt *cI = dyn_cast<ConstantInt>(c))
	{
		rtn.bottom = false;
		if(cI->isNegative()){
			// too large!
			if(cI->getBitWidth() > 32){
				rtn.upper = -std::numeric_limits<float>::infinity();
				rtn.lower = rtn.upper;
			}
			else{
				rtn.upper = (float) cI->getSExtValue();
				rtn.lower = rtn.upper;
			}
		}
		else{
			rtn.bottom = false;
			rtn.upper = (float) cI->getZExtValue();
			rtn.lower = rtn.upper;
		}		
	}
	if (ConstantFP *cF = dyn_cast<ConstantFP>(c))
	{
		rtn.bottom = false;
		rtn.upper = cF->getValueAPF().convertToFloat();
		rtn.lower = rtn.upper;		
	}
	return rtn;
}

// Note here we use "type" to distinguish int ("type=0") and float ("type=1")
RAFlow* RangeAnalysis::runOpInst(RAFlow* in, Instruction* instruction, unsigned opcode, int type) {
	RAFlow* f = new RAFlow(in);	
	Value *inst = instruction;

	Range lRange, rRange, rtnRange;
	Value *left = instruction->getOperand(0);
	Value *right = instruction->getOperand(1);

	ConstantInt *leftI = dyn_cast<ConstantInt>(left);
	ConstantInt *rightI = dyn_cast<ConstantInt>(right);
	ConstantFP *leftF = dyn_cast<ConstantFP>(left);
	ConstantFP *rightF = dyn_cast<ConstantFP>(right);

	//errs() << left->getName() << " " << right->getName() << "\n";
	// Deal with int operand
	if (type == 0){
		if (leftI) lRange = getConstRange(leftI, type);
		else{
			string name = "";
			if (left->getName()!="") name = left->getName();
			else name = "tmp";

			if (f->value.find(name) == f->value.end())
				errs() << "[left] undefined [int] variable: "<< left->getName() <<"\n";
			else
				lRange = f->value.find(name)->second;
			if (left->getName()=="") f->value.erase("tmp");

		}
		if (rightI) rRange = getConstRange(rightI, type);
		else{
			string name = "";
			if (right->getName()!="") name = right->getName();
			else name = "tmp";

			if (f->value.find(name) == f->value.end())
				errs() << "[right] undefined [int] variable: "<< right->getName() <<"\n";
			else
				rRange = f->value.find(name)->second;
			if (right->getName()=="") f->value.erase("tmp");
		}
	}
	// Deal with float operand
	if (type == 1){
		if (leftF) lRange = getConstRange(leftF, type);
		else{
			string name = "";
			if (left->getName()!="") name = left->getName();
			else name = "tmp";

			if (f->value.find(name) == f->value.end())
				errs() << "[left] undefined [float] variable: " << left->getName() <<"\n";
			else
				lRange = f->value.find(name)->second;
		    if (left->getName()=="") f->value.erase("tmp");
		}
		if (rightF) rRange = getConstRange(rightF, type);
		else{
			string name = "";
			if (right->getName()!="") name = right->getName();
			else name = "tmp";

			if (f->value.find(name) == f->value.end())
				errs() << "[right] undefined [float] variable: " << right->getName() <<"\n";
			else
				rRange = f->value.find(name)->second;
			if (right->getName()=="") f->value.erase("tmp");
		}
	}
	//errs() << lRange.lower << " "<< lRange.upper << "\n";
	//errs() << rRange.lower << " "<< rRange.upper << "\n";
	rtnRange = getRange(lRange, rRange, opcode);
	
	RAFlow* fnew = new RAFlow(0);
	//errs() << fnew->triPoint << " "<< f->triPoint<<"\n";
	map<string, Range> value;
	value[inst->getName()] = rtnRange;
	//value[inst->getName()] = rtnRange;
	fnew->value = value;
	f = static_cast<RAFlow*>(fnew->join(f));
	return f;
}

RAFlow* RangeAnalysis::runStoreInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);

	Range rtnRange;
	Value* left = instruction->getOperand(0);
	Value* right = instruction->getOperand(1);
	//errs() << left->getName() << " "<<right->getName() <<"\n";
	if (f->value.find(left->getName()) == f->value.end()){
		if (dyn_cast<Constant>(left)){
			if(ConstantInt *cI = dyn_cast<ConstantInt>(left))
				rtnRange = getConstRange(cI, 0);
			else if(ConstantFP *cP = dyn_cast<ConstantFP>(left))
				rtnRange = getConstRange(cP, 1);
		}
		else{
			errs() << "undefined variable: " <<left->getName() <<"\n";
		}
	}
	else{
		rtnRange = f->value[left->getName()];
		f->value.erase(left->getName());
	}
	if (f->value.find(right->getName()) != f->value.end())
		f->value.erase(right->getName());
	map<string, Range> value;
	//value[inst->getName()] = rtnRange;
	value[right->getName()] = rtnRange;
	RAFlow* fnew = new RAFlow(0);
	fnew->value = value;
	f = static_cast<RAFlow*>(fnew->join(f));
	//errs() << "runStoreInst:" << "[ " << rtnRange.lower << ", " << rtnRange.upper << " ]" << "\n";
	return f;

}

RAFlow* RangeAnalysis::runLoadInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);
	Range rtnRange;
	Value* input = instruction->getOperand(0);

	if (dyn_cast<Constant>(input)){
		if(ConstantInt *cI = dyn_cast<ConstantInt>(input))
			rtnRange = getConstRange(cI, 0);
		else if(ConstantFP *cP = dyn_cast<ConstantFP>(input))
			rtnRange = getConstRange(cP, 1);
	}
	else{
		if (f->value.find(input->getName()) == f->value.end())
			errs() << "undefined variable: " <<input->getName() <<"\n";
		else{
			rtnRange = f->value.find(input->getName())->second;
		}
	}

	map<string, Range> value;
	value["tmp"] = rtnRange;
	//errs() << "runCastInst:" << "[ " << rtnRange.lower << ", " << rtnRange.upper << " ]" << "\n";
	RAFlow* fnew = new RAFlow(0);
	fnew->value = value;
	f = static_cast<RAFlow*>(fnew->join(f));
	return f;
}

RAFlow* RangeAnalysis::runCompInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);	

	Value *left = instruction->getOperand(0);
	Value *right = instruction->getOperand(1);

	ConstantInt *leftI = dyn_cast<ConstantInt>(left);
	ConstantInt *rightI = dyn_cast<ConstantInt>(right);
	ConstantFP *leftF = dyn_cast<ConstantFP>(left);
	ConstantFP *rightF = dyn_cast<ConstantFP>(right);

	if ((!leftI) & (!leftF) & (left->getName()=="") & (f->value.find("tmp")!=f->value.end()))
		f->value.erase("tmp");
	if ((!rightI) & (!rightF) & (right->getName()=="") & (f->value.find("tmp")!=f->value.end()))
		f->value.erase("tmp");	
	return f;
}

RAFlow* RangeAnalysis::runRtnInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);

	Range rtnRange;
	Value* input = instruction->getOperand(0);

	if (dyn_cast<Constant>(input)){
		if(ConstantInt *cI = dyn_cast<ConstantInt>(input))
			rtnRange = getConstRange(cI, 0);
		else if(ConstantFP *cP = dyn_cast<ConstantFP>(input))
			rtnRange = getConstRange(cP, 1);
	}
	else{
		string name = "";
		if (input->getName()=="")
			name = "tmp";
		else
			name = input->getName();
		if (f->value.find(name) == f->value.end())
			errs() << "undefined variable: " <<name <<"\n";
		else{
			rtnRange = f->value.find(name)->second;
		}
		if (input->getName()=="") f->value.erase("tmp");
	}
	f->value["retval"] = rtnRange;

	return f;
}

RAFlow* RangeAnalysis::runCastInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);

	Range rtnRange;
	Value* input = instruction->getOperand(0);

	if (dyn_cast<Constant>(input)){
		if(ConstantInt *cI = dyn_cast<ConstantInt>(input))
			rtnRange = getConstRange(cI, 0);
		else if(ConstantFP *cP = dyn_cast<ConstantFP>(input))
			rtnRange = getConstRange(cP, 1);
	}
	else{
		if (f->value.find(input->getName()) == f->value.end())
			errs() << "undefined variable: " <<input->getName() <<"\n";
		else{
			rtnRange = f->value.find(input->getName())->second;
		}
	}

	map<string, Range> value;
	//value[inst->getName()] = rtnRange;
	value[input->getName()] = rtnRange;
	//errs() << "runCastInst:" << "[ " << rtnRange.lower << ", " << rtnRange.upper << " ]" << "\n";
	RAFlow* fnew = new RAFlow(0);
	fnew->value = value;
	f = static_cast<RAFlow*>(fnew->join(f));
	return f;
}


RAFlow* RangeAnalysis::runPhiInst(RAFlow* in, Instruction* instruction) {
	RAFlow* f = new RAFlow(in);
	Value *inst= instruction;

	Range lRange, rRange, maxRange;
	Value *left = instruction->getOperand(0);
	Value *right = instruction->getOperand(1);

	if(f->value.find(left->getName()) == f->value.end())
		lRange = getConstRange(left);
	else
		lRange = f->value.find(left->getName())->second;
	if(f->value.find(right->getName()) == f->value.end())
		rRange = getConstRange(right);
	else
		rRange = f->value.find(right->getName())->second;
	maxRange = JoinRange((const Range*) &lRange, (const Range*) &rRange);

	RAFlow* fnew = new RAFlow();
	map<string, Range> value;
	
	if (f->value.find(inst->getName()) != f->value.end())
	{
		lRange = maxRange;
		rRange = f->value[inst->getName()];
		maxRange = JoinRange((const Range*) &lRange, (const Range*) &rRange);
	}
	value[inst->getName()] = maxRange;
	fnew->value = value;
	f = static_cast<RAFlow*>(fnew->join(f));
	return f;
}

// Implementing data flow functions for arithmetic operands
Range RangeAnalysis::getRange(Range lRange, Range rRange, unsigned opcode){
	Range rtnRange;

	// Check if input elements have been analyzed
	if (rRange.undefined || lRange.undefined || rRange.top || lRange.top){
		rtnRange.top = true;
		rtnRange.bottom = false;
		rtnRange.upper = 0;
		rtnRange.lower = 0;
		return rtnRange;
	}
	// Operate range calculation case-by-case
	float tmp[4];
	switch (opcode) {
		case Instruction::Add:  
		case Instruction::FAdd:
			rtnRange.lower = lRange.lower + rRange.lower;
			rtnRange.upper = lRange.upper + rRange.upper;
			rtnRange.bottom = false;
			break;
		case Instruction::Sub:
		case Instruction::FSub:
			rtnRange.lower = lRange.lower - rRange.lower;
			rtnRange.upper = lRange.upper - rRange.upper;
			rtnRange.bottom = false;
			break;
		case Instruction::Mul:
		case Instruction::FMul:
			tmp[0] = lRange.lower * rRange.lower;
			tmp[1] = lRange.lower * rRange.upper;
			tmp[2] = lRange.upper * rRange.lower;
			tmp[3] = lRange.upper * rRange.upper;
			rtnRange.lower = tmp[0];
			rtnRange.upper = tmp[1];
			rtnRange.bottom = false;
			for(int i=1; i<4; i++){
				if (tmp[i]<rtnRange.lower) rtnRange.lower = tmp[i];
				if (tmp[i]>rtnRange.upper) rtnRange.upper = tmp[i];
			}
			break;
		case Instruction::SDiv:
		case Instruction::FDiv:
		    if (rRange.lower*rRange.upper>=0){
				tmp[0] = lRange.lower / rRange.lower;
				tmp[1] = lRange.lower / rRange.upper;
				tmp[2] = lRange.upper / rRange.lower;
				tmp[3] = lRange.upper / rRange.upper;
				rtnRange.lower = tmp[0];
				rtnRange.upper = tmp[1];
				rtnRange.bottom = false;
				for(int i=1; i<4; i++){
					if (tmp[i]<rtnRange.lower) rtnRange.lower = tmp[i];
					if (tmp[i]>rtnRange.upper) rtnRange.upper = tmp[i];
				}
		    }
		    break;
		case Instruction::URem:
		case Instruction::SRem:
		case Instruction::FRem:		
			tmp[0] = (int) lRange.lower % (int) rRange.lower;
			tmp[1] = (int) lRange.lower % (int) rRange.upper;
			tmp[2] = (int) lRange.upper % (int) rRange.lower;
			tmp[3] = (int) lRange.upper % (int) rRange.upper;
			rtnRange.lower = tmp[0];
			rtnRange.upper = tmp[1];
			rtnRange.bottom = false;
			for(int i=1; i<4; i++){
				if (tmp[i]<rtnRange.lower) rtnRange.lower = tmp[i];
				if (tmp[i]>rtnRange.upper) rtnRange.upper = tmp[i];
			}
			break;
		case Instruction::Shl:
			tmp[0] = (int) lRange.lower << (int) rRange.lower;
			tmp[1] = (int) lRange.lower << (int) rRange.upper;
			tmp[2] = (int) lRange.upper << (int) rRange.lower;
			tmp[3] = (int) lRange.upper << (int) rRange.upper;
			rtnRange.lower = tmp[0];
			rtnRange.upper = tmp[1];
			rtnRange.bottom = false;
			for(int i=1; i<4; i++){
				if (tmp[i]<rtnRange.lower) rtnRange.lower = tmp[i];
				if (tmp[i]>rtnRange.upper) rtnRange.upper = tmp[i];
			}
			break;
		case Instruction::AShr:
			tmp[0] = (int) lRange.lower >> (int) rRange.lower;
			tmp[1] = (int) lRange.lower >> (int) rRange.upper;
			tmp[2] = (int) lRange.upper >> (int) rRange.lower;
			tmp[3] = (int) lRange.upper >> (int) rRange.upper;
			rtnRange.lower = tmp[0];
			rtnRange.upper = tmp[1];
			rtnRange.bottom = false;
			for(int i=1; i<4; i++){
				if (tmp[i]<rtnRange.lower) rtnRange.lower = tmp[i];
				if (tmp[i]>rtnRange.upper) rtnRange.upper = tmp[i];
			}
			break;
	}
	return rtnRange;
}


void RangeAnalysis::print(raw_ostream &OS) {
    for (unsigned int i = 0; i < CFGNodes.size() ; i++) {
        this->printHelper(OS,this->CFGNodes[i]);
        if(i+1 < CFGNodes.size()) {
            OS << "\n";
        }
        OS << "\n";
    }
}

void RangeAnalysis::printHelper(raw_ostream &OS, LatticeNode* node) {
    
	OS << "representation : " << *(node->inst) << "\n";
	OS << "#Edge incoming" << "\n";
    for (unsigned int i = 0 ; i < node->incoming.size() ; i++) {
		RAFlow * tmp = (RAFlow*)node->incoming[i]->flow;
		OS << tmp->toString() << "\n";
    }
 	OS << "#Edge outcoming" << "\n";
    for (unsigned int i = 0 ; i < node->outgoing.size() ; i++) {
		RAFlow * tmp = (RAFlow*)node->outgoing[i]->flow;
		OS<<tmp->toString()<<"\n";
    }
}

Flow * RangeAnalysis::initialize() {
	return new RAFlow(BOTTOM);
}

RangeAnalysis::RangeAnalysis(Function & F){
	this->functionName = F.getName();
    CFGmaker(F);
}
