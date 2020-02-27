#include "adder.h"

// function adds operand 1 and 2 and
// returns result. use local variable for computation
// in order to get result immediately ( -> signal semantics)
void adder::compute(){
    // read values of input port   
    int op1 = a.read();
    int op2 = b.read();
    int res;      
    res = op1 + op2;
    // write result to ouput port
    c.write(res);      
	// or simply use:
	// c.write(a.read()+ b.read());
}
