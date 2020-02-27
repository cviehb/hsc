# script running cordic high level synthesis
open_project HLS_Cordic
set_top cc_cordicModule_fixed
add_files cc_cordicModule_fixed.cpp
add_files -tb testbench_cc_cordicModule_fixed.cpp
# add_files -tb hls_platform_test.cpp
open_solution "HLS_Cordic"
set_part {xc7z007sclg225-2} -tool vivado
create_clock -period 10 -name default
#source "./HLS_Cordic/HLS_Cordic/directives.tcl"
csim_design -ldflags {-I/usr/local/systemc-2.3.2/include/ -L/usr/local/systemc-2.3.2/lib/ -lsystemc} -setup
csynth_design
#cosim_design -ldflags {-I/usr/local/systemc/include -lm -lsystemc}
# export_design -format ip_catalog

