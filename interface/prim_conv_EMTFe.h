//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
//
// Create Date:    17:17:10 03/12/2010
// Design Name: tf_slhc
// Module Name:    csc_prim_conv
// Project Name: tf_slhc
// Target Devices: V6
// Tool versions: 11.4
// Description: primitive converter into angular values
//
// Dependencies:
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments: Translated by Rafael into C++
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef _PRIM_CONV_CLASS_
#define _PRIM_CONV_CLASS_

//#define PC_PRINT_MACRO
#define RPC_PRINT_MACRO

#ifndef __SYNTHESIS__
  #ifdef PC_PRINT_MACRO
    #include <iostream>
  #endif
#endif
#ifdef _LOCAL_AP_INT_H_
  #include "../include/ap_int.h"
#else
  #include <ap_int.h>
#endif
#include "../interface/spbits.h"

void csc_prim_conv(
    // input parameters from MPC
    const ap_uint<seg_ch>   &vpf,                  // valid flags
    const ap_uint<bw_wg>     wiregroup[seg_ch],    // wiregroup numbers
    const ap_uint<bw_hs>     hstrip   [seg_ch],    // halfstrip numbers
    const ap_uint<4>         clctpat  [seg_ch],    // clct pattern numbers
          ap_uint<6>         th_mem   [th_mem_sz], // makes memory size such that any address will be in range
          ap_uint<13>        params   [6],         // programmable parameters [0] = ph_init, [1] = th_init, [2] = ph_displacement, [3] = th_displacement
    const ap_uint<4>         st,
    const ap_uint<4>         ch,
    const ap_uint<13>        r_in,
    const ap_uint<1>         we,
    const ap_uint<bw_addr>   addr,
    const ap_uint<2>         sel,
    // outputs
          ap_uint<bw_fph>    ph[seg_ch],           // phi
          ap_uint<bw_th>     th[seg_ch],           // full precision th, but without displacement correction
          ap_uint<seg_ch>   &vl,                   // one-bit valid flags
          ap_uint<7>        &phzvl,                // raw hit valid flags for up to 3 ph zones
          ap_uint<seg_ch>   &me11a,
          ap_uint<4>         clctpat_r[seg_ch],    // clct pattern numbers
          //ap_uint<ph_hit_w> &ph_hit,               // ph and th raw hits
          ap_uint<13>       &r_out,
    const ap_uint<1>        &endcap,
    const ap_uint<1>        &lat_test              // latency test mode
);

void rpc_prim_conv(
    // input parameters from MPC
    const ap_uint<bw_fph-2>  ph_rpc[seg_ch],       /* input  */
    const ap_uint<bw_th-2>   th_rpc[seg_ch],       /* input  */
    const ap_uint<4>         st,
    const ap_uint<4>         ch,
    const ap_uint<seg_ch>   &vpf,                  // valid flags
    // outputs
          ap_uint<bw_fph>    ph[seg_ch],           // phi
          ap_uint<bw_th>     th[seg_ch],           // full precision th, but without displacement correction
          ap_uint<seg_ch>   &vl,                   // one-bit valid flags
          ap_uint<7>        &phzvl,                // raw hit valid flags for up to 3 ph zones
    //ap_uint<ph_hit_w> &ph_hit,               // ph and th raw hits
    const ap_uint<1>        &lat_test              // latency test mode
);

#endif

