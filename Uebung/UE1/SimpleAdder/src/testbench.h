#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc.h>
#include <iostream>
#include <math.h>
#include <cstdlib>

using namespace std;

// testbench for adder
 SC_MODULE(testbench){
     sc_in<int>  result;
     sc_out<int> op1;
     sc_out<int> op2;

     sc_in<int>  res_add3;
     sc_out<int> op1_add3;
     sc_out<int> op2_add3;
     sc_out<int> op3_add3;


     void stimulus();

     SC_CTOR(testbench){
         SC_THREAD(stimulus);
     }
  };

#endif
