#include "WorkList.h"

void WorkList::CFGmaker(Function &F){
    //DFS clone graph without recursion! yes
    //algorithm is at http://bangbingsyb.blogspot.com/2014/11/leetcode-clone-graph.html
    map<Instruction *, LatticeNode *> map;
    stack <Instruction *>  s;
    int index = 1;
    
    
    Function::iterator BB = F.begin();
	BasicBlock::iterator BI = BB->begin();
    Instruction * instruction = &*(BI); 
    LatticeNode * first = new LatticeNode(index++);
    first->inst = instruction;
    this->root = first;
    this->CFGNodes.push_back(first);
    map[instruction] =  first;
    s.push(instruction);
    
    
    //make the fist edge for firstNode
    LatticeEdge* firstEdge = new LatticeEdge(*CFGNodes.begin(),*CFGNodes.begin());
   	(*CFGNodes.begin())->incoming.push_back(firstEdge);
   	this->CFGEdges.push_back(firstEdge);


    while(!s.empty()) {
        Instruction * currentInst = s.top();
        LatticeNode * curNode = map[currentInst];
        s.pop();


        //child instruct
        vector <Instruction *> childsInst;
        //Value *refValue = currentInst;
        if(isa<BranchInst>(currentInst)) {
            BranchInst * branchInst = dyn_cast<BranchInst>(currentInst);
            for ( unsigned int i = 0 ; i < branchInst->getNumSuccessors() ; i++) {
            BasicBlock * bb = branchInst->getSuccessor(i);
            Instruction * nextInst = &*(bb->begin());
            if(nextInst != NULL) childsInst.push_back(nextInst);
            }
        //not branch
        } else {
               //not end
               //add following inst
                if (currentInst->getNextNode()!=0) childsInst.push_back(currentInst->getNextNode());
            }
        //childs are ready, start dfs
        for (unsigned int i =0; i < childsInst.size(); i++) {
            Instruction * currentChildInst = childsInst[i];

            if(map.count(currentChildInst)) {
                //this inst has never been
                LatticeNode * childNode = map[currentInst];
                LatticeEdge * newEdge = new LatticeEdge(curNode,childNode);
                curNode->outgoing.push_back(newEdge);
                childNode->incoming.push_back(newEdge);
                this->CFGEdges.push_back(newEdge);
            } else {
                //create new Node which is not visited
                LatticeNode * newNode = new LatticeNode(index++);
                newNode->inst = currentChildInst;
                LatticeEdge * newEdge = new LatticeEdge(curNode, newNode);
                curNode->outgoing.push_back(newEdge);
                newNode->incoming.push_back(newEdge);
                map[currentChildInst]=newNode;
                s.push(currentChildInst);
                this->CFGNodes.push_back(newNode);
                this->CFGEdges.push_back(newEdge);
            }

        }
       
    }
}


void WorkList::runWorklist() {
	//same in the slide
	queue<LatticeNode*> worklist;

	for (unsigned int i = 0; i < CFGEdges.size(); i++) {
		CFGEdges[i]->flow = initialize();
	}

	//Add to worklist
	for (unsigned int i = 0 ; i < CFGNodes.size(); i++) {
		worklist.push(CFGNodes[i]);
	}

	while(!worklist.empty()){
		
		LatticeNode* current = worklist.front();
		errs()<<"----------------> start working on " <<*current->inst<<" <------\n";

		vector<Flow*> inputFlows;
		for (unsigned int i = 0 ; i < current->incoming.size() ; i++) {
			inputFlows.push_back(current->incoming[i]->flow);
		}
        
		
		Flow* in = initialize();
		in->copy(inputFlows[0]);
		for (unsigned int i = 1 ; i < inputFlows.size(); i++){
			Flow* f = in->join(inputFlows[i]);
			delete in; 
			in = f;
		}

	    //executeFlowFunction which is override by subclass
		Flow* out = executeFlowFunction(in,	current->inst,current->index);	


		
		for(unsigned int i = 0 ; i < current->outgoing.size(); i++) {
	
			Flow* new_out = out->join(current->outgoing[i]->flow);
			//if output changes, push back
			errs()<< current->index << " : new_out: "<<new_out->triPoint<<"\n";
			if (!(new_out->equals(current->outgoing[i]->flow))){
				errs()<< current->index << "flow was" << current->outgoing[i]->flow->triPoint<< "change to ===> " << " new_out: "<<new_out->triPoint<<"\n";
				current->outgoing[i]->flow->copy(new_out);
				worklist.push(current->outgoing[i]->dst);
			}
		}
		worklist.pop();
	}
}


void WorkList::print(raw_ostream &OS) {

	OS<<"you shall not pass"<<"\n";
}

Flow* WorkList::initialize(){
	return new Flow(BOTTOM);
}


Flow* WorkList::executeFlowFunction(Flow* in, Instruction *inst, int NodeId)
{

	return new Flow(TOP);
}

WorkList::WorkList(Function &F){

	this->functionName = F.getName();
	CFGmaker(F);

}

WorkList::WorkList() {}

WorkList::~WorkList(){

}
