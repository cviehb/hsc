#ifndef bus_spec
#define bus_spec

#include <array>
#include <sysc/datatypes/fx/sc_fixed.h>
#include <sysc/datatypes/fx/sc_ufixed.h>
#include <systemc.h>

namespace bus_spec {
int const transactions = 1000000;
int const max_cycles = 10;
int const memory_size = 3;

int const offset_phi = 0;
int const offset_ctrl = 1;
int const offset_xy = 2;

int const rdy = 1;

size_t const phi_size = 22;
size_t const xcos_size = 16;
size_t const ysin_size = 16;
size_t const calc_count = 16;

typedef std::array<int, memory_size> memory_t;

typedef sc_signal<sc_dt::sc_ufixed<phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                                   sc_dt::sc_o_mode::SC_SAT>>
    phi_t;
typedef sc_signal<sc_dt::sc_ufixed<xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                                   sc_dt::sc_o_mode::SC_SAT>>
    sincos_t;

typedef sc_in<sc_dt::sc_ufixed<phi_size, 1, sc_dt::sc_q_mode::SC_RND,
                               sc_dt::sc_o_mode::SC_SAT>>
    phi_t_i;
typedef sc_out<sc_dt::sc_ufixed<xcos_size, 0, sc_dt::sc_q_mode::SC_RND,
                                sc_dt::sc_o_mode::SC_SAT>>
    sincos_t_o;

sc_time transport_delay = sc_time(10, SC_NS);
sc_time cycle_delay = sc_time(10, SC_NS);
}; // namespace bus_spec

#endif
