#!/bin/bash

echo "Copying files, make sure lates version is used!"
cp ../tlm_block_cordic/cc_cordicModule_fi* .
cp ../tlm_block_cordic/bus_spec.h .

echo "Running HLS ..."
vivado_hls -f run_cordic_hls.tcl
