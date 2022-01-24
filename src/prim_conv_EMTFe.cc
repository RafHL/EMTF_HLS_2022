//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
//
// Create Date:    17:17:10 03/12/2010
// Design Name: tf_slhc
// Module Name:    csc_prim_conv (rpc_prim_conv further down)
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

#include "../interface/prim_conv_EMTFe.h"

void csc_prim_conv(
    // input parameters from MPC
    const ap_uint<seg_ch>   &vpf,                  // valid flags
    const ap_uint<bw_wg>     wiregroup[seg_ch],    // wiregroup numbers
    const ap_uint<bw_hs>     hstrip   [seg_ch],    // halfstrip numbers
    const ap_uint<4>         clctpat  [seg_ch],    // clct pattern numbers
          ap_uint<6>         th_mem   [th_mem_sz], // makes memory size such that any address will be in range
          ap_uint<13>        params   [6],         // programmable parameters [0] = ph_init, [1] = th_init, [2] = ph_displacement, [3] = th_displacement
    const ap_uint<4>         station,
    const ap_uint<4>         cscid,
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
) {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS pipeline
#pragma HLS latency max=1 // For visible latency warnings
    // Completely partition variable array into registers
#pragma HLS array_partition variable=params //cyclic factor=1
    // \/ /\ cyclic 4 worked best
#pragma HLS array_partition variable=th_mem //cyclic factor=1
//#pragma HLS inline off

    #include "../interface/ph_init_hard.h"
#pragma HLS array_partition variable=ph_init_hard dim=0
    #include "../interface/zns_bnd.h"
#pragma HLS array_partition variable=zns_bnd dim=0

    ap_uint<7>          phzvl_tmp = 0;
    ap_uint<bw_fph>     fph[seg_ch];            // initial ph for this chamber scaled to 0.1333 deg/strip
    ap_uint<bw_fph-5>   ph_hard = ph_init_hard[station][cscid]; // select chamber start for this unit from the table above
    ap_uint<bw_fph>     fph_to_reduce[2];
    ap_uint<bw_fph-5>   ph_reduced[2];

    ap_uint<3>          clct_pat_corr;           // phi correction derived from clct pattern
    ap_uint<1>          clct_pat_sign;           // phi correction sign

    // eighth-strip
    ap_uint<bw_fph>     eight_str[seg_ch];
    ap_uint<mult_bw>    mult;
    ap_uint<bw_fph>     ph_tmp;
    ap_uint<bw_wg>      wg;
    ap_uint<bw_th>      th_tmp;
    ap_uint<seg_ch>     vl_tmp = 0; // Tmp variable to reduce interval to 1
    // theta lut, takes wiregroup, returns theta

    //ph_hit = 0;

    // from spbits.sv::113-123
    // ph zone boundaries for chambers that cover more than one zone
    // hardcoded boundaries must match boundaries in ph_th_match module
    // calc parameters based on station and cscid
    //const unsigned int ph_zone_bnd1_c = ph_zone_bnd1;
    //const unsigned int ph_zone_bnd2_c = ph_zone_bnd2;
    const ap_uint<4> st = (station == ap_uint<4>(0)) ? ap_uint<4>(1) : station; // ME11a and ME11b are station 1 in Jia Fu's array

    // is this chamber mounted in reverse direction?
    const ap_uint<1>  ph_reverse = (endcap == 0 && station >= 3) ? 1 :
                                   (endcap == 1 && station <  3) ? 1 : 0;

    // strip width factor relative to ME234/2
    // 1024 == 1
    const ap_uint<11> factor = (station <= 1 && ((cscid >= 6 && cscid <= 8) || (cscid == 14))) ? 947 : // ME1/3
                                                                                                 1024; // all other chambers

    if (we == 0) {
        for (unsigned int i = 0; i < seg_ch; i++) {
            // clct pattern convertion array from CMSSW
            //{0.0, 0.0, -0.60,  0.60, -0.64,  0.64, -0.23,  0.23, -0.21,  0.21, 0.0}
            // 0    0    -5      +5    -5      +5    -2      +2    -2      +2    0
            switch(clctpat[i]) {
                case 0  :  clct_pat_corr = 0; clct_pat_sign = 0; break;
                case 1  :  clct_pat_corr = 0; clct_pat_sign = 0; break;
                case 2  :  clct_pat_corr = 5; clct_pat_sign = 1; break;
                case 3  :  clct_pat_corr = 5; clct_pat_sign = 0; break;
                case 4  :  clct_pat_corr = 5; clct_pat_sign = 1; break;
                case 5  :  clct_pat_corr = 5; clct_pat_sign = 0; break;
                case 6  :  clct_pat_corr = 2; clct_pat_sign = 1; break;
                case 7  :  clct_pat_corr = 2; clct_pat_sign = 0; break;
                case 8  :  clct_pat_corr = 2; clct_pat_sign = 1; break;
                case 9  :  clct_pat_corr = 2; clct_pat_sign = 0; break;
                case 10 :  clct_pat_corr = 0; clct_pat_sign = 0; break;
                default :  clct_pat_corr = 0; clct_pat_sign = 0; break;
            }

            // Commented in original code
            // reverse clct pattern correction if chamber is reversed
            //if (ph_reverse) clct_pat_sign = ~clct_pat_sign;

            // 10 deg chambers
            if (
                    (station <= 1) || // ME1 all 10 deg
                    (station >= 2 && ((cscid >= 3 && cscid <= 8) || cscid  == 10)) // ME2,3,4 outer ring
               ) {
                // full precision, uses only 2 bits of clct pattern correction
                eight_str[i] = (ap_uint<2>(0), hstrip[i], ap_uint<2>(0));
                if      (clct_pat_sign == 0) eight_str[i] += clct_pat_corr(2,1);
                else if (eight_str[i]  != 0) eight_str[i] -= clct_pat_corr(2,1); // fix on Jia Fu request 2016-11-10
            } else {
                // 20 deg chambers
                // multiply by 2, uses all 3 bits of pattern correction
                eight_str[i] = (ap_uint<1>(0), hstrip[i], ap_uint<3>(0));
                if      (clct_pat_sign == 0) eight_str[i] += clct_pat_corr;
                else if (eight_str[i]  != 0) eight_str[i] -= clct_pat_corr; // fix on Jia Fu request 2016-11-10
            }

            if (vpf[i]) {
                vl_tmp[i] = 1;
                // ph conversion
                // for factors 1024 and 2048 the multiplier should be replaced with shifts by synthesizer
                mult = eight_str[i] * factor;
                ph_tmp = mult(mult_bw-1, 10);
                if (ph_reverse) fph[i] = params[0] - ph_tmp;
                else            fph[i] = params[0] + ph_tmp;

                fph_to_reduce[i] = fph[i] + 16; // +16 to put the rounded value into the middle of error range
                // divide full ph by 32, subtract chamber start
                ph_reduced[i] = fph_to_reduce[i](bw_fph-1,5) - ph_hard;
                //ph_hit[ph_reduced[i]] = 1; // set hit in zone

                wg = wiregroup[i];
                // th conversion
                th_tmp = th_mem[wg];
                th[i] = th_tmp + params[1];
                if (th[i] == 0) th[i] = 1; // protect against invalid value


                // check which zones ph hits should be applied to
                // Wait to see what to do about the hit with no zone
                for (int zn = 0; zn < 3; zn++)
    #pragma HLS unroll
                    if (
                        th[i] >= zns_bnd[1][st][zn][0] &&
                        th[i] <= zns_bnd[1][st][zn][1]
                    ) phzvl_tmp[2-zn] = 1; // Bit reversed by Jia Fu request 8/13/21

                clctpat_r[i] = clctpat[i]; // just propagate pattern downstream
            } else if (lat_test == 1 && cscid == 0 && i == 0) { // 218-229
                // in latency test mode, generate fake stub in chamber 0
                vl_tmp[i]  = 1;
                fph[i]     = 2048;
                //ph_hit[22] = 1;
                th[i]      = 30; // ~17 deg
                phzvl_tmp  = 1;
                clctpat_r[i] = 0;
            } else {
                vl_tmp[i]  = 0;
                fph[i]     = 0;
                clctpat_r[i] = 0;
                th[i]      = 0;
                //phzvl_tmp  = 0;
            }
            ph[i] = fph[i];
        }

    } else {
        if      (sel == 0) params[addr]      =            r_in;
        else if (sel == 1) th_mem[addr]      = ap_uint<6>(r_in);
    }

    r_out = (sel == 0) ?             params[addr]  :
            (sel == 1) ? ap_uint<13>(th_mem[addr]) :
                         (ap_uint<5>(0), station, cscid);

    //Assign outputs
    me11a = 0;
    vl = vl_tmp;
    phzvl = phzvl_tmp;

#ifndef __SYNTHESIS__
    for (unsigned int i = 0; i < seg_ch; i++)
        if (vl[i] != 0 && phzvl == 0)
            std::cout << "Warning: PC/PC11 hit not in any zone: st: " << station << " cscid: " << cscid << " th: " << th[i] << std::endl;
#define PC_PRINT_MACRO
#ifdef PC_PRINT_MACRO
    for (unsigned int i = 0; i < seg_ch; i++) {
        if (vl[i] != 0) {
            std::cout << "    st:" << station
                      << "    ch:" << cscid
                      << "    ph:" << ph[i]
                      << "    th:" << th[i]
                      << "    ph_reduced: " << ph_reduced[i]
                      << std::endl;
        }
    }
#endif // PC_PRINT
#endif // Synthesis
}

//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
//
// Create Date:    17:17:10 03/12/2010
// Design Name: tf_slhc
// Module Name:    rpc_prim_conv
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

void rpc_prim_conv(
    // input parameters from MPC
    const ap_uint<bw_fph-2>  ph_rpc[seg_ch],       /* input  */
    const ap_uint<bw_th-2>   th_rpc[seg_ch],       /* input  */
    const ap_uint<4>         station,
    const ap_uint<4>         rpcid,
    const ap_uint<seg_ch>   &vpf,                  // valid flags
    // outputs
          ap_uint<bw_fph>    ph[seg_ch],           // phi
          ap_uint<bw_th>     th[seg_ch],           // full precision th, but without displacement correction
          ap_uint<seg_ch>   &vl,                   // one-bit valid flags
          ap_uint<7>        &phzvl,                // raw hit valid flags for up to 3 ph zones
    //ap_uint<ph_hit_w> &ph_hit,               // ph and th raw hits
    const ap_uint<1>        &lat_test              // latency test mode
) {

#pragma HLS interface ap_ctrl_none port=return
    //#pragma HLS inline off
#pragma HLS pipeline
#pragma HLS latency max=1 // For visible latency warnings
#pragma HLS array_partition variable=ph_rpc
#pragma HLS array_partition variable=th_rpc

#include "../interface/ph_init_hard.h"
#pragma HLS array_partition variable=ph_init_hard dim=0
#include "../interface/zns_bnd.h"
#pragma HLS array_partition variable=zns_bnd dim=0

    ap_uint<7>          phzvl_tmp = 0;
    ap_uint<bw_fph-5>   ph_hard = ph_init_hard[station][rpcid]; // select chamber start for this unit from the table above
    ap_uint<bw_fph>     fph_to_reduce[2];
    ap_uint<bw_fph-5>   ph_reduced[2];

    ap_uint<seg_ch>     vl_tmp = 0; // Tmp variable to reduce interval to 1

    //ph_hit = 0;

    // from spbits.sv::113-123
    // ph zone boundaries for chambers that cover more than one zone
    // hardcoded boundaries must match boundaries in ph_th_match module
    // calc parameters based on station and rpcid
    //const unsigned int ph_zone_bnd1_c = ph_zone_bnd1;
    //const unsigned int ph_zone_bnd2_c = ph_zone_bnd2;
    const ap_uint<4> st = (station == ap_uint<4>(0)) ? ap_uint<4>(1) : station; // ME11a and ME11b are station 1 in Jia Fu's array

    for (unsigned int i = 0; i < seg_ch; i++) {
        ph[i] = (ph_rpc[i], ap_uint<2>(0));
        if (vpf[i]) {
            vl_tmp[i] = 1;

            fph_to_reduce[i] = ph_rpc[i] + 16; // +16 to put the rounded value into the middle of error range
            // divide full ph by 32
            ph_reduced[i] = fph_to_reduce[i](bw_fph-1,5) - ph_hard;
            //ph_hit[ph_reduced[i]] = 1; // set hit in zone


            // th conversion
            if (th_rpc[i] == 0)
                th[i] = 1; // protect against invalid value
            else
                th[i] = (th_rpc[i], ap_uint<2>(0));


            // check which zones ph hits should be applied to
            for (int zn = 0; zn < 3; zn++)
            #pragma HLS unroll
                if (
                    th[i] >= zns_bnd[2][st][zn][0] &&
                    th[i] <= zns_bnd[2][st][zn][1]
                ) phzvl_tmp[2-zn] = 1; // Bit reversed by Jia Fu request 8/13/21

        } else if (lat_test == 1 && rpcid == 0 && i == 0) { // 218-229
            // in latency test mode, generate fake stub in chamber 0
            vl_tmp[i]  = 1;
            //ph_hit[22] = 1;
            phzvl_tmp  = 1;
            th[i] = 0;
        } else {
            vl_tmp[i]  = 0;
            //phzvl_tmp  = 0;
            th[i] = 0;
        }
    }

    //Assign outputs
    phzvl = phzvl_tmp;
    vl = vl_tmp;

#ifndef __SYNTHESIS__
    for (unsigned int i = 0; i < seg_ch; i++)
        if (vpf[i] != 0 && phzvl == 0)
            std::cout << "Warning: RPC hit not in any zone: st: " << station << " rpcid: " << rpcid << " th: " << th[i] << " th_rpc: " << th_rpc[i] << std::endl;
    #ifdef RPC_PRINT_MACRO
    for (unsigned int i = 0; i < seg_ch; i++) {
        if (vpf[i] != 0) {
            std::cout << "    rst:" << station << "    rch:" << rpcid << "    ";
            std::cout << "rph:" << ph_rpc[i] << "    ";
            std::cout << "rth:" << th_rpc[i];
            std::cout << std::endl;
        }
    }
    #endif // PC_PRINT
#endif // Synthesis
}

