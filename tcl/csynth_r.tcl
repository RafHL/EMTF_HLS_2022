############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLS_project_VerfnLinked
set_top sp_tl
add_files interface/zones.h -cflags "-std=c++0x"
add_files src/zones.cc -cflags "-std=c++0x"
add_files interface/spbits.h -cflags "-std=c++0x"
add_files interface/sp.h -cflags "-std=c++0x"
add_files src/sp.cc -cflags "-std=c++0x"
add_files interface/prim_conv_sector.h -cflags "-std=c++0x"
add_files src/prim_conv_sector.cc -cflags "-std=c++0x"
add_files interface/prim_conv11.h -cflags "-std=c++0x"
add_files src/prim_conv11.cc -cflags "-std=c++0x"
add_files interface/prim_conv.h -cflags "-std=c++0x"
add_files src/prim_conv.cc -cflags "-std=c++0x"
add_files src/sp.cc -cflags "-std=c++0x"
add_files tops_Rafael/prim_conv_tl.cpp -cflags "-std=c++0x"
add_files tb/hex_to_ap.h -cflags "-std=c++0x"
add_files tb/fill_params.h -cflags "-std=c++0x"
add_files src/sp_tl.cc -cflags "-std=c++0x"
add_files -tb tb/sp_tb.cc -cflags "-Wno-unknown-pragmas -std=c++0x"
open_solution "solution1"
set_part {xcku5p-ffvd900-3-e} -tool vivado
create_clock -period 20 -name default
csynth_design
quit

