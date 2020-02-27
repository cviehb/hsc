#ifndef ADDER_H_
#define ADDER_H_


#include <systemc.h>


// a simple ader module
SC_MODULE(adder){
   sc_in<int>    a;
   sc_in<int>    b;
   sc_out<int>   c;
   
   void compute();
   
   SC_CTOR(adder): a("op1"), b("op2"), c("res"){
       SC_METHOD(compute);
         sensitive << a << b;
         dont_initialize();
    }
 };

// hierarchical module
 SC_MODULE(Add3){
    sc_in<int>    in1;
    sc_in<int>    in2;
    sc_in<int>    in3;
    sc_out<int>   sum;
    // internal signal
    sc_signal<int> tmp;
    // adder instances
    adder * pAdd1;
    adder * pAdd2;
    
    SC_CTOR(Add3){
       pAdd1 = new adder("adder1");
       // positional form (order is very important)
       (*pAdd1)(in1, in2, tmp);
       
       pAdd2 = new adder("adder2");
       // access via pointer (named form
       // order does not matter)
       pAdd2->a(tmp);
       pAdd2->b(in3);
       pAdd2->c(sum);
    }

    ~Add3(){
        delete pAdd1; 
        pAdd1 = 0;

        delete pAdd2;
        pAdd2 = 0;
    }
 };

#endif
