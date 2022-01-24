############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project eHLS_Vivado_project
set_top sp
add_files   interface/spbits.h                   -cflags "-std=c++0x"
add_files   interface/sp_EMTFe.h                 -cflags "-std=c++0x"
add_files         src/sp_EMTFe.cc                -cflags "-std=c++0x"
add_files   interface/prim_conv_sector_EMTFe.h   -cflags "-std=c++0x"
add_files         src/prim_conv_sector_EMTFe.cc  -cflags "-std=c++0x"
add_files   interface/prim_conv11_EMTFe.h        -cflags "-std=c++0x"
add_files         src/prim_conv11_EMTFe.cc       -cflags "-std=c++0x"
add_files   interface/prim_conv_EMTFe.h          -cflags "-std=c++0x"
add_files         src/prim_conv_EMTFe.cc         -cflags "-std=c++0x"
add_files         src/coord_delay.cc                  -cflags "-std=c++0x"
add_files emtf-hlslib/firmware/myproject.cpp     -cflags "-std=c++11"
#add_files tops_Rafael/sp_tl_EMTFe.cc             -cflags "-std=c++0x"
open_solution "solution1"
set_part {xcku025-ffva1156-1-c}
create_clock -period 20 -name default
csynth_design
quit

