#include "testbench.h"

// used to generate input values for adder
int testbench::quad(int a, int b){
     return (int)pow((double)a, (double)b);
 }

// stimuli function
 void testbench::stimulus(){

     int a = 0;
     int b = 0;
     int res = 0;

     cout << "test simple adder" << endl;
     
     wait(10, SC_NS);
	 // generate new input
     op1 = 5;
     op2 = 4;
	 // wait so computation can be done
     wait(10, SC_NS);
     // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;
	
	 // generate new input
     op1 = 0;
     op2 = 3;
	 // wrong values are read because of driver semantics
     cout << "test driver of signals: expected 0 and 3, but got " << op1.read() << " and " << op2.read() << endl;
	 // wait so computation can be done
     wait(10, SC_NS);
     // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = 0;
     op2 = 0;

     wait(10, SC_NS);

     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     
     op1 = quad(2,32);
     op2 = 1;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = -2;
     op2 = -3;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = 2^16;
     op2 = 2^21;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = -2^31;
     op2 = 3;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = -2;
     op2 = 3;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     op1 = 1;
     op2 = -1;
	 // wait so computation can be done
     wait(10, SC_NS);     
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1.read() << "+" << op2.read() << "=" << result.read() << endl;

     assert(res ==(a+b));

     wait(10, SC_NS);

     cout << "end of testing simple adder." << endl;
     cout << "----------------------------" << endl;
     cout << "now testing add3 module." << endl;

     wait(10, SC_NS);

     op1_add3 = 5;
     op2_add3 = 4;
     op3_add3 = 0;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1_add3.read() << "+" << op2_add3.read() << "+" << op3_add3.read() << "=" << res_add3.read() << endl;

     op1_add3 = 5;
     op2_add3 = 4;
     op3_add3 = -10;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1_add3.read() << "+" << op2_add3.read() << "+" << op3_add3.read() << "=" << res_add3.read() << endl;

     op1_add3 = 0;
     op2_add3 = 0;
     op3_add3 = 0;
	 // wait so computation can be done
     wait(10, SC_NS);
	 // print result
     cout << sc_time_stamp() << "\t"
          << " adder result: " << op1_add3.read() << "+" << op2_add3.read() << "+" << op3_add3.read() << "=" << res_add3.read() << endl;
    
     cout << "end of test!" << endl;

 }