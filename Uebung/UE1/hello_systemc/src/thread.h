#ifndef THREAD_H
#define THREAD_H

#include <systemc.h>

class ProcessB : public sc_module {
public:
  sc_in<bool> itrigger;

  SC_CTOR(ProcessB);

 private:
  void doWork();
};

#endif
