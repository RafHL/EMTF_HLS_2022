//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
//
// Create Date:    01:23:48 03/27/2010 
// Design Name: tf_slhc
// Module Name:    sp
// Project Name: tf_slhc
// Target Devices: V6
// Tool versions: 11.4
// Description: sector processor top module
//
// Dependencies: all underlying modules
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: Translated by Rafael into C++
//
//////////////////////////////////////////////////////////////////////////////////
#ifndef _SP_CLASS_
#define _SP_CLASS_

#ifdef _LOCAL_AP_INT_H_
  #include "../include/ap_int.h"
#else
  #include <ap_int.h>
#endif
#include "spbits.h"
#include "prim_conv_sector_EMTFe.h"
#include "coord_delay.h"
#include "../emtf-hlslib/firmware/myproject.h"

//#define PC_PRINT_MACRO

/* Cosimulation: need self-check test bench (ie can write to a file and see that
 * output file == golden using diff, array comparisons, etc)
 * ap_ctrl_none is fine for pipelined functions
 * I have been doing this myself, but it may be better to do it later
 * Was thinking: -D to_log and write C++ outputs to file, then in cosim check
 * against these ideal values from C++ output
 */

void sp(
    // [station][chamber][segment] station 5 = neighbor sector, all stations
    // unpack LCTs in Verilog code -- unable to do so here without increasing II
    const ap_uint<seg_ch>   (&vpf)   [12][9], // 6 CSC, 5 RPC
    const ap_uint<4>        (&q)     [12][9][seg_ch],
    const ap_uint<bw_wg>    (&wg)    [6][9][seg_ch],
    const ap_uint<bw_hs>    (&hstr)  [6][9][seg_ch],
    const ap_uint<4>        (&cpat)  [6][9][seg_ch],
//    const ap_uint<4>        (&th_corr_mem)[3][3][th_corr_mem_sz],
//    const ap_uint<6>        (&th_mem)[6][9][th_mem_sz],
//    const ap_uint<13>       (&params)[6][9][6],
    // Alex says I can assume RPC <> CSC already comes in decoded
    const ap_uint<bw_fph-2> (&ph_rpc)[6][9][seg_ch],
    const ap_uint<bw_th-2>  (&th_rpc)[6][9][seg_ch],
    const ap_uint<6>        (&time)  [12][9][seg_ch], // TODO: Dimmensions recheck, RPC input
    const ap_uint<10>       (&bend)  [12][9][seg_ch],

          ap_int<14>        (&model_out)[emtf::phase2::model_config::n_out],
          ap_uint<13>        &r_out,

#ifdef GOLD_FULL
          ap_uint<bw_fph>  (&ph)   [12][9][seg_ch],
          ap_uint<bw_th>   (&th11) [3][3][th_ch11],
          ap_uint<bw_th>   (&th)   [12][9][seg_ch],
          ap_uint<seg_ch>  (&vl)   [12][9],
          ap_uint<7>       (&phzvl)[12][9],
          ap_uint<seg_ch>  (&me11a)[3][3],
          ap_uint<4>       (&cpatr)[6][9][seg_ch],
#endif

    const ap_uint<1>         &endcap,
    const ap_uint<3>         &sector,
    const ap_uint<1>         &lat_test,
    const ap_uint<64>        &core_config,
    const ap_uint<13>         r_in,
    const ap_uint<1>          we,
    const ap_uint<bw_addr>    addr,
    const ap_uint<2>          sel,
    const ap_uint<9>        (&cs)[6]
);

#endif
