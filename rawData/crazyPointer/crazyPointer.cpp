
#define NULL 0

int main() {
	int a;
	int b;
	int c;
	int d;
	int * massive_p1;
	int x;
	int y;
	int z;
	int *crazy_p2;
	massive_p1 = &a;
	massive_p1 = &b;
	massive_p1 = &c;
	massive_p1 = &d;
	crazy_p2 = &x;
	crazy_p2 = &y;
	crazy_p2 = &z;
		
	massive_p1=&z;
	crazy_p2= &a;

 
	return 0;
}