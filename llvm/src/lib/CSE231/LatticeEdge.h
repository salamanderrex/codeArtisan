#ifndef LATTICE_EDGE
#define LATTICE_EDGE
#include <string>
#include "Flow.h"
#include "LatticeNode.h"
#include "llvm/Support/raw_ostream.h"
class LatticeNode;
class LatticeEdge {
public:
    Flow * flow;
    LatticeNode * src;
    LatticeNode * dst;
    LatticeEdge(LatticeNode * src, LatticeNode * dst)  {
        this->src = src;
        this->dst = dst;
        flow = new Flow();
            
    }
    ~LatticeEdge() {
        delete flow;
    }
 
};


#endif
