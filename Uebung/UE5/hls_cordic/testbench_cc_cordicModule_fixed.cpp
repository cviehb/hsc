// Shows the direct memory interfaces and the DMI hint.
// Shows the debug transaction interface
// Shows the proper use of response status

#include "cc_cordicModule_fixed.h"
#include <systemc.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

int sc_main(int argc, char *agrv[]) {
  sc_clock cc_sig_clk("clk", 1, SC_NS);
  sc_signal<bool> cc_sig_nrst;
  bus_spec::phi_t cc_sig_phi;
  bus_spec::sincos_t cc_sig_xcos;
  bus_spec::sincos_t cc_sig_ysin;

  sc_signal<bool> sig_start;
  sc_signal<bool> cc_sig_rdy;

  cc_cordicModule_fixed *ccCordic;
  ccCordic = new cc_cordicModule_fixed("cccordic");

  ccCordic->phi(cc_sig_phi);
  ccCordic->xcos(cc_sig_xcos);
  ccCordic->ysin(cc_sig_ysin);
  ccCordic->clk(cc_sig_clk);
  ccCordic->nrst(cc_sig_nrst);
  ccCordic->start(sig_start);
  ccCordic->rdy(cc_sig_rdy);

  sc_start();

  return 0;
}
