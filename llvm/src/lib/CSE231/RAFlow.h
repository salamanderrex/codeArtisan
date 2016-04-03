#include "llvm/Support/raw_ostream.h"
#include "WorkList.h"
#include "Flow.h"
#include <map>


using namespace llvm;
using namespace std;

typedef struct _Range{
	float upper;
	float lower;
	bool top;
	bool bottom;
	bool undefined;
	_Range()
	{
		upper = 0;
		lower = 0;
		top = false;
		bottom = true; // start with bottom
		undefined = false;
	}
}Range;


class RAFlow: public Flow{
	public:
		bool equals(Flow* other);
		void copy(Flow* rhs);
		Flow* join(Flow* other);
		RAFlow();
		RAFlow(int triPoint);
		RAFlow(RAFlow* flow);
		~RAFlow();

		map<string, Range> value;

		string toString();
};
Range JoinRange(const Range* left, const Range* right);
bool RangeEqual(const Range* left, const Range* right);
