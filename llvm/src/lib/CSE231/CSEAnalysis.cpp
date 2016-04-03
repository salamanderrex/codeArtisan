
#include "CSEAnalysis.h"


bool CSEAnalysis::isEqual(CSEAnalysisFlow* in,
		Instruction *inst) {

	typedef std::map<std::string, std::string>::iterator it_type;


	unsigned opcode = inst->getOpcode();

	switch (opcode) {

	case Instruction::Trunc:
	case Instruction::ZExt:
	case Instruction::SExt:
	case Instruction::FPToSI:
	case Instruction::FPToUI:
	case Instruction::UIToFP:
	case Instruction::SIToFP:
	case Instruction::FPTrunc:
	case Instruction::FPExt:
	case Instruction::SRem:
		return true;
		break;

	}
	return false;

}

Flow* CSEAnalysis::executeFlowFunction(Flow *in,
		Instruction *inst, int NodeId) {
	CSEAnalysisFlow* inFlow =
			static_cast<CSEAnalysisFlow*>(in);
	CSEAnalysisFlow * output;

	switch (inst->getOpcode()) {

	case Instruction::Add:
	case Instruction::Sub:
	case Instruction::Mul:
	case Instruction::SDiv:
	case Instruction::SRem:
	case Instruction::Shl:
	case Instruction::LShr:
	case Instruction::AShr:
		output = runOpInst(inFlow, inst, inst->getOpcode());
		break;
	case Instruction::FAdd:
	case Instruction::FSub:
	case Instruction::FMul:
	case Instruction::FDiv:
	case Instruction::FRem:
		output = runFOpInst(inFlow, inst, inst->getOpcode());
		break;

	case Instruction::Trunc:
	case Instruction::ZExt:
	case Instruction::SExt:
	case Instruction::FPToUI:
	case Instruction::FPToSI:
	case Instruction::UIToFP:
	case Instruction::SIToFP:
	case Instruction::FPTrunc:
	case Instruction::FPExt:
		output = runUnaryInst(inFlow, inst, inst->getOpcode());
		break;
	case Instruction::PHI:
		output = runPhiInst(inFlow, inst);
		break;

	default:
		output = new CSEAnalysisFlow(inFlow);
		break;
	}

	return output;
}

Flow * CSEAnalysis::initialize() {
	return new CSEAnalysisFlow(BOTTOM);
}

void CSEAnalysis::print(raw_ostream &OS) {

    for (unsigned int i = 0; i < CFGNodes.size() ; i++) {
        this->printHelper(OS,this->CFGNodes[i]);
        if(i+1 < CFGNodes.size()) {
            OS << "\n";
        }
            OS << "\n";
    }
}


void CSEAnalysis::printHelper(raw_ostream &OS, LatticeNode* node) {
    
	OS << "representation : " << *(node->inst) << "\n";
	OS << "#Edge incoming" << "\n";
    for (unsigned int i = 0 ; i < node->incoming.size() ; i++) {
 
		CSEAnalysisFlow * temp = (CSEAnalysisFlow * )node->incoming[i]->flow;
		OS << temp->arrowList() << "\n";

    }
	OS << "\n";
	
 	OS << "#Edge outcoming" << "\n";
    for (unsigned int i = 0 ; i < node->outgoing.size() ; i++) {

		CSEAnalysisFlow * temp = (CSEAnalysisFlow * )node->outgoing[i]->flow;
		OS<<temp->arrowList()<<"\n";
 
    }

}

CSEAnalysisFlow* CSEAnalysis::runUnaryInst(
		CSEAnalysisFlow* in, Instruction* instruction,
		unsigned opcode) {

	CSEAnalysisFlow* f = new CSEAnalysisFlow(in);

	map<string, string> value;
	Value *retVal = instruction;
	string regName = retVal->getName();

	Value* casting = instruction->getOperand(0); 

	if (!dyn_cast<Constant>(retVal)) {

		if (!dyn_cast<Constant>(casting)) {

			string resVal = computeUnaryOp(
					"%" + std::string(casting->getName()), opcode);
			if (f->value.find("%" + std::string(retVal->getName()))
					== f->value.end()) {
				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
		
				value["%" + std::string(retVal->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}

		} else {

			if (ConstantFP *CILeft = dyn_cast<ConstantFP>(casting)) {

				float leftVal = CILeft->getValueAPF().convertToFloat();

				std::ostringstream ss;
				ss << leftVal;
				std::string sl(ss.str());
				string resVal = computeUnaryOp(sl, opcode);

				if (f->value.find("%" + std::string(retVal->getName()))
						== f->value.end()) {

					CSEAnalysisFlow* f_ =new CSEAnalysisFlow();

					value["%" + std::string(retVal->getName())] = resVal;

					f_->value = value;
					CSEAnalysisFlow* tmp =
							static_cast<CSEAnalysisFlow*>(f_->join(
									f));
					delete f_;
					delete f;
					f = tmp;

				}
			} else if (ConstantInt *CILeft = dyn_cast<ConstantInt>(casting)) {
				int leftVal = CILeft->getZExtValue();

				std::ostringstream ss;
				ss << leftVal;
				std::string sl(ss.str());
				string resVal = computeUnaryOp(sl, opcode);

				if (f->value.find("%" + std::string(retVal->getName()))
						== f->value.end()) {

					CSEAnalysisFlow* f_ =
							new CSEAnalysisFlow();

					value["%" + std::string(retVal->getName())] = resVal;

					f_->value = value;
					CSEAnalysisFlow* tmp =
							static_cast<CSEAnalysisFlow*>(f_->join(
									f));
					delete f_;
					delete f;
					f = tmp;
				}

			}

		}

	}

	return f;

}

string CSEAnalysis::computeUnaryOp(string leftVal,
		unsigned opcode) {

	string resVal = "";

	std::ostringstream ss;

	switch (opcode) {

	case Instruction::Trunc:
		ss << "trunk";
		break;
	case Instruction::ZExt:
		ss << "zext";
		break;
	case Instruction::SExt:
		ss << "sext";
		break;
	case Instruction::FPToSI:
		ss << "fptosi";
		break;
	case Instruction::FPToUI:
		ss << "fptoui";
		break;
	case Instruction::UIToFP:
		ss << "uitofp";
		break;
	case Instruction::SIToFP:
		ss << "sitofp";
		break;
	case Instruction::FPTrunc:
		ss << "fptrunc";
		break;
	case Instruction::FPExt:
		ss << "fpext";
		break;
	case Instruction::SRem:
		ss << "srem";
		break;
	}

	ss << " ";
	ss << leftVal;

	std::string s(ss.str());

	resVal = s;
	return resVal;

}

string CSEAnalysis::computeBinaryOp(string leftVal,
		string rightVal, unsigned opcode) {

	string resVal = "";

	std::ostringstream ss;

	switch (opcode) {

	case Instruction::Add:
		ss << "add";
		break;
	case Instruction::FAdd:
		ss << "fadd";
		break;
	case Instruction::Sub:
		ss << "sub";
		break;
	case Instruction::FSub:
		ss << "fsub";
		break;
	case Instruction::FDiv:
		ss << "fdiv";
		break;
	case Instruction::SDiv:
		ss << "sdiv";
		break;
	case Instruction::FMul:
		ss << "fmul";
		break;
	case Instruction::Mul:
		ss << "mul";
		break;
	case Instruction::FRem:
		ss << "frem";
		break;
	case Instruction::SRem:
		ss << "srem";
		break;
	case Instruction::Shl:
		ss << "shl";
		break;
	case Instruction::LShr:
		ss << "lshl";
		break;
	case Instruction::AShr:
		ss << "ashr";
		break;
	}

	ss << " ";
	ss << leftVal;
	ss << ", ";
	ss << rightVal;
	std::string s(ss.str());

	resVal = s;
	return resVal;

}

CSEAnalysisFlow* CSEAnalysis::runPhiInst(
		CSEAnalysisFlow* in, Instruction* instruction) {

	CSEAnalysisFlow* f = new CSEAnalysisFlow(
			in);
	Value *leftOperand = instruction->getOperand(0);
	Value *rightOperand = instruction->getOperand(1);
	map<string, string> value;
	Value *K = instruction;
	string regName = K->getName();

	if ((f->value.find("%" + std::string(leftOperand->getName()))
			== f->value.end())
			| (f->value.find("%" + std::string(rightOperand->getName()))
					== f->value.end())) {
	} else {
	
		string leftVal = f->value.find(
				"%" + std::string(leftOperand->getName()))->second;

		string rightVal = f->value.find(
				"%" + std::string(rightOperand->getName()))->second;

		this->isEqual(in, instruction);
		if (leftVal == rightVal) {

			string resVal = leftVal;
			CSEAnalysisFlow* f_ =
					new CSEAnalysisFlow();

			value["%" + std::string(K->getName())] = resVal;
			f_->value = value;
			CSEAnalysisFlow* tmp =
					static_cast<CSEAnalysisFlow*>(f_->join(f));
			delete f_;
			delete f;
			f = tmp;
		}

	}
	return f;
}

CSEAnalysisFlow* CSEAnalysis::runFOpInst(
		CSEAnalysisFlow* in, Instruction* instruction,
		unsigned opcode) {

	CSEAnalysisFlow* f = new CSEAnalysisFlow(
			in);
	Value *leftOperand = instruction->getOperand(0);
	Value *rightOperand = instruction->getOperand(1);
	map<string, string> value;
	Value *K = instruction;
	string regName = K->getName();

	if (ConstantFP *CILeft = dyn_cast<ConstantFP>(leftOperand)) {

		float leftVal = CILeft->getValueAPF().convertToFloat();
		if (ConstantFP *CIRight = dyn_cast<ConstantFP>(rightOperand)) {


			float rightVal = CIRight->getValueAPF().convertToFloat();

			std::ostringstream ss;
			ss << leftVal;
			std::string sl(ss.str());

			std::ostringstream ssr;
			ssr << rightVal;
			std::string sr(ssr.str());

			string resVal = computeBinaryOp(sl, sr, opcode);

			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {

				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();

				value["%" + std::string(K->getName())] = resVal;

				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}
		} else {

			std::ostringstream ss;
			ss << leftVal;
			std::string sl(ss.str());
			string sr = rightOperand->getName();

			string resVal = computeBinaryOp(sl, "%" + sr, opcode);

			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {

				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}
		}
	} else {

		string sl = leftOperand->getName();

		if (ConstantFP *CIRight = dyn_cast<ConstantFP>(rightOperand)) {

			float rightVal = CIRight->getValueAPF().convertToFloat();

			std::ostringstream ssr;
			ssr << rightVal;
			std::string sr(ssr.str());

			string resVal = computeBinaryOp("%" + sl, sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {
				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
		
				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}


		} else {

			string sr = rightOperand->getName();
			string resVal = computeBinaryOp("%" + sl, "%" + sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {
				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}

		}

	}
	return f;
}

CSEAnalysisFlow* CSEAnalysis::runOpInst(
		CSEAnalysisFlow* in, Instruction* instruction,
		unsigned opcode) {

	CSEAnalysisFlow* f = new CSEAnalysisFlow(
			in);
	Value *leftOperand = instruction->getOperand(0);
	Value *rightOperand = instruction->getOperand(1);
	map<string, string> value;
	Value *K = instruction;
	string regName = K->getName();

	if (ConstantInt *CILeft = dyn_cast<ConstantInt>(leftOperand)) {

		float leftVal = CILeft->getZExtValue();
		if (ConstantInt *CIRight = dyn_cast<ConstantInt>(rightOperand)) {

			float rightVal = CIRight->getZExtValue();

			std::ostringstream ss;
			ss << leftVal;
			std::string sl(ss.str());

			std::ostringstream ssr;
			ssr << rightVal;
			std::string sr(ssr.str());

			string resVal = computeBinaryOp(sl, sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {

				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();

				value["%" + std::string(K->getName())] = resVal;

				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}
		} else {

			std::ostringstream ss;
			ss << leftVal;
			std::string sl(ss.str());
			string sr = rightOperand->getName();
			string resVal = computeBinaryOp(sl, "%" + sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {
				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}
		}
	} else {

		string sl = leftOperand->getName();

		if (ConstantInt *CIRight = dyn_cast<ConstantInt>(rightOperand)) {

			float rightVal = CIRight->getZExtValue();

			std::ostringstream ssr;
			ssr << rightVal;
			std::string sr(ssr.str());

			string resVal = computeBinaryOp("%" + sl, sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {

				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();

				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}


		} else {

			string sr = rightOperand->getName();
			string resVal = computeBinaryOp("%" + sl, "%" + sr, opcode);
			if (f->value.find("%" + std::string(K->getName()))
					== f->value.end()) {

				CSEAnalysisFlow* f_ =
						new CSEAnalysisFlow();
				value["%" + std::string(K->getName())] = resVal;
				f_->value = value;
				CSEAnalysisFlow* tmp =
						static_cast<CSEAnalysisFlow*>(f_->join(
								f));
				delete f_;
				delete f;
				f = tmp;
			}

		}

	}
	return f;
}


CSEAnalysis::CSEAnalysis(Function & F) :
		WorkList() {

	this->functionName = F.getName();
	CFGmaker(F);

}
