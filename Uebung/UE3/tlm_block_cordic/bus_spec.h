#ifndef bus_spec
#define bus_spec

#include <array>
#include <systemc.h>

namespace bus_spec {
int const transactions = 1000000;
int const max_cycles = 10;
int const memory_size = 3;

int const offset_phi = 0;
int const offset_ctrl = 1;
int const offset_xy = 2;

int const rdy = 1;

typedef std::array<int, memory_size> memory_t;

sc_time transport_delay = sc_time(10, SC_NS);
sc_time cycle_delay = sc_time(10, SC_NS);
}; // namespace bus_spec

#endif
