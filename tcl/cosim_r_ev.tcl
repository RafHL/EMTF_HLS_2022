############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project eHLS_Vivado_project
set_top sp
#add_files   interface/spbits.h                   -cflags "-std=c++0x"
#add_files   interface/sp_EMTFe.h                 -cflags "-std=c++0x"
add_files         src/sp_EMTFe.cc                -cflags "-std=c++0x -D GOLD"
#add_files   interface/prim_conv_sector_EMTFe.h   -cflags "-std=c++0x"
add_files         src/prim_conv_sector_EMTFe.cc  -cflags "-std=c++0x"
#add_files   interface/prim_conv11_EMTFe.h        -cflags "-std=c++0x"
add_files         src/prim_conv11_EMTFe.cc       -cflags "-std=c++0x"
#add_files   interface/prim_conv_EMTFe.h          -cflags "-std=c++0x"
add_files         src/prim_conv_EMTFe.cc         -cflags "-std=c++0x"
add_files          tb/hex_to_ap.h                -cflags "-std=c++0x"
add_files          tb/fill_params.h              -cflags "-std=c++0x"
add_files emtf-hlslib/firmware/myproject.cpp     -cflags "-std=c++11" -csimflags "-Wall -Wextra -Wno-unused-label -Wno-unused-parameter"
add_files -tb      tb/sp_tb_EMTFe.cc             -cflags "-Wno-unknown-pragmas -std=c++0x -D GOLD"
open_solution "solution1"
set_part {xcku025-ffva1156-1-c}
create_clock -period 20 -name default
# Twice in case GOLD file not available
if { [file exists "sp_tf_gold.out"] == 0 } {
    puts "\nGold reference file doesn't exist. Running twice, 1st to make sp_tf_gold.out, 2nd to check against gold reference file\n"
    cosim_design -argv "../../../../emtf_data/"
}
cosim_design -argv "../../../../emtf_data/"
quit

