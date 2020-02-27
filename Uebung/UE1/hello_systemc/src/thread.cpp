#include "thread.h"

SC_HAS_PROCESS(ProcessB);

ProcessB::ProcessB(sc_module_name name) : sc_module(name) {
  SC_THREAD(doWork);
}

void ProcessB::doWork() {
  while (true) {
    wait(itrigger.value_changed_event());
    std::cout << "Method" << name() << "triggered" 
	      << sc_time_stamp() << std::endl;
    
  }
}

