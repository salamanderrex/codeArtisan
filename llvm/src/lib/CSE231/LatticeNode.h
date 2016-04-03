#ifndef LATTICE_NODE
#define LATTICE_NODE
#include <vector>
#include "Flow.h"
#include "LatticeEdge.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
class LatticeEdge;
class LatticeNode {
public:
    int index;
    std::vector<LatticeEdge *> incoming;
    std::vector<LatticeEdge *> outgoing;
    Instruction * inst;
    
    
    LatticeNode() {

    }
    LatticeNode(int index ) {
        this->index = index;
    }
    void print() {
        errs()<<"LatticeNode index is "<<index<<"\n";
        
    }

};

#endif
