int main() {
	int hiddenValue;
	int * y_other;
	int * x;
	int ** y;
	y_other=&hiddenValue;
	y= &y_other;
	

	x=*y;
}
