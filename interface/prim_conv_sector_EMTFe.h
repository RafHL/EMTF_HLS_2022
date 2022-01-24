//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Engineer: Madorsky
// Translator: Rafael Hernandez
//
// Create Date:    01:36:08 03/19/2010
// Design Name: tf_slch
// Module Name:    prim_conv_sector
// Project Name: tf_slch
// Target Devices: V6
// Tool versions: 11.4
// Description: primitive converter for entire sector
//
// Dependencies: prim_conv
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments: Translated by Rafael into C++
//
//////////////////////////////////////////////////////////////////////////////////
#ifndef _PRIM_CONV_SECTOR_CLASS_
#define _PRIM_CONV_SECTOR_CLASS_

// Undef and define new changes into a new include file
#ifdef _LOCAL_AP_INT_H_
  #include "../include/ap_int.h"
#else
  #include <ap_int.h>
#endif
#include "spbits.h"
#include "prim_conv_EMTFe.h"
#include "prim_conv11_EMTFe.h"


void prim_conv_sector(
    // lct parameters [station][chamber][segment]
    // st 5 = neighbor sector, all stations
    const ap_uint<seg_ch>    vpf   [12][9],              /* input  */
    const ap_uint<bw_wg>     wg    [6][9][seg_ch],       /* input  */
    const ap_uint<bw_hs>     hstr  [6][9][seg_ch],       /* input  */
    const ap_uint<4>         cpat  [6][9][seg_ch],       /* input  */
//    const ap_uint<4>         th_corr_mem[3][3][th_corr_mem_sz], /* input */
//    const ap_uint<6>         th_mem[6][9][th_mem_sz],    /* input  */
//    const ap_uint<13>        params[6][9][6],            /* input  */
    const ap_uint<bw_fph-2>  ph_rpc[6][9][seg_ch],       /* input  */
    const ap_uint<bw_th-2>   th_rpc[6][9][seg_ch],       /* input  */
          ap_uint<bw_fph>    ph    [12][9][seg_ch],       /* output */
    // special th outputs for ME11 because of duplication
    // [station][chamber][segment with duplicates], station 2 = neighbor segment
          ap_uint<bw_th>     th11  [3][3][th_ch11],      /* output */
          ap_uint<bw_th>     th    [12][9][seg_ch],       /* output */ // TODO: 13,9
          ap_uint<seg_ch>    vl    [12][9],               /* output */ // TODO: 13,9
          ap_uint<7>         phzvl [12][9],              /* output */
    // me11a flags only for ME11 (stations 1,0, chambers 2:0)
    // [station][chamber][segment], station 2 = neighbor segment
          ap_uint<seg_ch>    me11a [3][3],               /* output */
          ap_uint<4>         cpatr [6][9][seg_ch],       /* output */
    // ph and th raw hits
          ////TODO: Fix first index once I am sure about the RPC geometry
          // Moved to Jia Fu's HLS portion
          //ap_uint<ph_hit_w>  ph_hit[11][9],              /* output */
          ap_uint<13>       &r_out,
    const ap_uint<1>        &endcap,                     /* input  */
    const ap_uint<1>        &lat_test,                   /* input  */
    const ap_uint<13>        r_in,
    const ap_uint<1>         we,
    const ap_uint<bw_addr>   addr,
    const ap_uint<2>         sel,
    const ap_uint<9>         cs[6]
);

#endif

