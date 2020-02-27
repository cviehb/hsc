#include <systemc.h>

#include "module.h"
#include "thread.h"

#include <vector>
#include <list>

using namespace std;

int sc_main(int argc, char *argv[])
{
  sc_clock clk("clk", 10, SC_NS);
  sc_signal<bool> trigger;

  Module *pm = new Module("MyModule");
  ProcessB *pt = new ProcessB("My_Thread");

  sc_clock ts("rst");
  
  pm->iclk(clk);
  pm->otrigger(trigger);
  pt->itrigger(trigger);
  
  sc_start(100, sc_time_unit::SC_NS);

  return 0;
}


