#ifndef MODULE_H
#define MODULE_H

#include <systemc.h>

class Module : public sc_module {
public:
  // define io ports
  sc_in<bool> iclk;
  sc_out<bool> otrigger;
  
  SC_CTOR(Module) {
    SC_METHOD(doWork) {
      sensitive << iclk.pos();
      
    }
  }
  
private:
  void doWork() {
    std::cout << "Method" << name() << "triggered" 
	      << sc_time_stamp() << std::endl;
  }
};

#endif
