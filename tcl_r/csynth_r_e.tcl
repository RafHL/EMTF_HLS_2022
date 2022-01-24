############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project eHLS_project_VerfnLinked
#set_top phps_tl
#set_top php_tl
set_top pze_tl
add_files   interface/ph_pattern_EMTFe.h         -cflags "-std=c++0x"
add_files         src/ph_pattern_EMTFe.cc        -cflags "-std=c++0x"
add_files   interface/ph_pattern_sector_EMTFe.h  -cflags "-std=c++0x"
add_files         src/ph_pattern_sector_EMTFe.cc -cflags "-std=c++0x"
add_files   interface/extend_sector_EMTFe.h      -cflags "-std=c++0x"
add_files         src/extend_sector_EMTFe.cc     -cflags "-std=c++0x"
add_files   interface/extender_EMTFe.h           -cflags "-std=c++0x"
add_files         src/extender_EMTFe.cc          -cflags "-std=c++0x"
add_files   interface/zones_EMTFe.h              -cflags "-std=c++0x"
add_files         src/zones_EMTFe.cc             -cflags "-std=c++0x"
add_files   interface/spbits.h                   -cflags "-std=c++0x"
add_files   interface/prim_conv_sector_EMTFe.h   -cflags "-std=c++0x"
add_files         src/prim_conv_sector_EMTFe.cc  -cflags "-std=c++0x"
add_files   interface/prim_conv11_EMTFe.h        -cflags "-std=c++0x"
add_files         src/prim_conv11_EMTFe.cc       -cflags "-std=c++0x"
add_files   interface/prim_conv_EMTFe.h          -cflags "-std=c++0x"
add_files         src/prim_conv_EMTFe.cc         -cflags "-std=c++0x"
add_files          tb/hex_to_ap.h                -cflags "-std=c++0x"
add_files          tb/fill_params.h              -cflags "-std=c++0x"
add_files tops_Rafael/pze_EMTFe.cc               -cflags "-std=c++0x"
#add_files tops_Rafael/php_EMTFe.cc               -cflags "-std=c++0x"
#add_files tops_Rafael/phps_EMTFe.cc               -cflags "-std=c++0x"
add_files -tb      tb/sp_tb_EMTFe.cc             -cflags "-Wno-unknown-pragmas -std=c++0x"
open_solution "solution1"
set_part {xcku025-ffva1156-1-c} -tool vivado
create_clock -period 20 -name default
csynth_design
quit

