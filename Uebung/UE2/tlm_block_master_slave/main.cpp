// Shows the direct memory interfaces and the DMI hint.
// Shows the debug transaction interface
// Shows the proper use of response status

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include "top.h"


int sc_main(int argc, char *agrv[]){
    top top("top");
    sc_start();
    return 0;
}
