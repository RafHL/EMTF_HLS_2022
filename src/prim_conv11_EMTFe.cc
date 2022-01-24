//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
//
// Create Date:    17:17:10 03/12/2010
// Design Name: tf_slhc
// Module Name:    prim_conv
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
// Try to localize temporary variables as much as possible
// Try to write and read same variable as little as possible

#include "../interface/prim_conv11_EMTFe.h"

void csc_prim_conv11(
    // input parameters from MPC
    const ap_uint<seg_ch>   &vpf,                    // valid flags
    const ap_uint<bw_wg>     wiregroup[seg_ch],      // wiregroup numbers
    const ap_uint<bw_hs>     hstrip   [seg_ch],      // halfstrip numbers
    const ap_uint<4>         clctpat  [seg_ch],      // clct pattern numbers
    // theta correction lut, for me1/1 only
    // takes index = {wiregroup(2 MS bits), dblstrip}, returns theta correction for tilted wires
          ap_uint<4>         th_corr_mem[th_corr_mem_sz],
          ap_uint<6>         th_mem     [th_mem_sz], // makes memory size such that any address will be in range
          ap_uint<13>        params     [6],
    const ap_uint<4>         station,
    const ap_uint<4>         cscid,
    const ap_uint<13>        r_in,
    const ap_uint<1>         we,
    const ap_uint<bw_addr>   addr,
    const ap_uint<2>         sel,
    // low-precision ph, only for detection
    // high-precision ph with displacement correction will be calculated when
    // 3 best tracks are found.
          ap_uint<bw_fph>    ph[seg_ch],
    // full precision th, but without displacement correction, takes th
    //     duplication into account
          ap_uint<bw_th>     th[th_ch11],            // Use worst case
          ap_uint<seg_ch>   &vl,                     // one-bit valid flags
          ap_uint<7>        &phzvl,                  // raw hit valid flags for up to 3 ph zones
          ap_uint<seg_ch>   &me11a,
          ap_uint<4>         clctpat_r[seg_ch],      // clct pattern numbers
          //ap_uint<ph_hit_w> &ph_hit,                 // ph and th raw hits
          ap_uint<13>       &r_out,
    const ap_uint<1>        &endcap
) {

#pragma HLS interface ap_ctrl_none port=return // Removes control signals
#pragma HLS pipeline // Fully pipelined, defaults to II=1
#pragma HLS latency max=1 // At most 1 clock cycle, and warn if more
//#pragma HLS inline off // inline off in case recursive inline in wrappers above

    // 9191 LUTs, 110 FF (params registers)
    //  929 LUTs,  60 FF (params cyclic 4)
#pragma HLS array_partition variable=params
#pragma HLS array_partition variable=th_mem
#pragma HLS array_partition variable=th
#pragma HLS array_partition variable=th_corr_mem

    #define th_coverage  46 // Jia Fu request 2016-11-03
    //#define th_negative = 50;

    ap_uint<bw_fph>     eight_str[seg_ch];
    ap_uint<mult_bw>    mult; // Always assigned before use
    //ap_uint<bw_wg + 5> wg_mult;
    ap_uint<bw_fph>     ph_tmp;
    ap_uint<bw_wg>      wg;

    // i, j for looping - placed within loop
    ap_uint<bw_th>      th_tmp, thf;
    // theta lut, takes wiregroup, returns theta

    // initial ph for this chamber scaled to 0.1333 deg/strip
    ap_uint<bw_fph>     fph[seg_ch];

    // signals only for ME1/1
    // have to declare them here since vppc does not support declarations in generate blocks yet
    ap_uint<4>          th_corr;
    ap_uint<bw_ds+2>    index;
    ap_uint<6>          th_orig;
    ap_uint<seg_ch>     vl_tmp = 0;
    ap_uint<7>          phzvl_tmp = 0;

    ap_uint<11>         factor[seg_ch]; // strip width factors for each segment

    //const ap_uint<8>    ph_zone_bnd1_c = ph_zone_bnd1; // From spbits
    //const ap_uint<8>    ph_zone_bnd2_c = ph_zone_bnd2; // From spbits
    const ap_uint<4> st = (station == ap_uint<4>(0)) ? ap_uint<4>(1) : station; // ME11a and ME11b are station 1 in Jia Fu's array

    ap_uint<3>          clct_pat_corr; // phi correction derived from clct pattern
    ap_uint<1>          clct_pat_sign; // phi correction sign
    ap_uint<3>          ph_init_ix; // parameter index for ph_init

    /* This is a debug variable from prim_conv11
    ap_uint<7>          th_tmp1[2];
    */

    #include "../interface/ph_init_hard.h"
#pragma HLS array_partition variable=ph_init_hard dim=0
    #include "../interface/zns_bnd.h"
#pragma HLS array_partition variable=zns_bnd dim=0


    // select chamber start for this unit from the table above
    const ap_uint<bw_fph-5> ph_hard = ph_init_hard[station][cscid];
    ap_uint<bw_fph-5>       ph_reduced[2];
    ap_uint<bw_fph>         fph_to_reduce[2];

    //ph_hit = 0;
    //ap_uint<ph_hit_w>   ph_hit_tmp = 0; // For initialization to 0 in case bit not written to below

    // is this chamber mounted in reverse direction?
    const ap_uint<1>    ph_reverse = (endcap == 0u && station >= 3u) ? 1u :
                                     (endcap == 1u && station <  3u) ? 1u : 0u;

    if (we == 0) {
        // calculate eight_str and me11a parameters upfront
        // bug found by Jia Fu on 2016-09-16
        for (unsigned int i = 0; i < seg_ch; i++) {
            // May want to look into HLS resource for the DSPs generated by this
            factor[i] = (station <= 1u && (cscid <= 2u || cscid == 12u) && hstrip[i] > 127u) ? 1707u : // ME1/1a
                 1301u; // ME1/1b

            //{0.0, 0.0, -0.60,  0.60, -0.64,  0.64, -0.23,  0.23, -0.21,  0.21, 0.0}
            // 0    0    -5      +5    -5      +5    -2      +2    -2      +2    0
            switch (clctpat[i]) {
                case 0 : clct_pat_corr = 0u; clct_pat_sign = 0u; break;
                case 1 : clct_pat_corr = 0u; clct_pat_sign = 0u; break;
                case 2 : clct_pat_corr = 5u; clct_pat_sign = 1u; break;
                case 3 : clct_pat_corr = 5u; clct_pat_sign = 0u; break;
                case 4 : clct_pat_corr = 5u; clct_pat_sign = 1u; break;
                case 5 : clct_pat_corr = 5u; clct_pat_sign = 0u; break;
                case 6 : clct_pat_corr = 2u; clct_pat_sign = 1u; break;
                case 7 : clct_pat_corr = 2u; clct_pat_sign = 0u; break;
                case 8 : clct_pat_corr = 2u; clct_pat_sign = 1u; break;
                case 9 : clct_pat_corr = 2u; clct_pat_sign = 0u; break;
                case 10: clct_pat_corr = 0u; clct_pat_sign = 0u; break;
                default: clct_pat_corr = 0u; clct_pat_sign = 0u; break;
            }

            // reverse clct pattern correction if chamber is reversed
            // if (ph_reverse) clct_pat_sign = ~clct_pat_sign;


            // convert into 1/8 strips and remove ME1/1a offset (512=128*4)
            me11a[i] = (station <= 1u && (cscid <= 2u || cscid == 12u) && hstrip[i] > 127u);
            eight_str[i] = (ap_uint<2>(0), hstrip[i], ap_uint<2>(0)) - (me11a[i] ? 512u : 0u);

            // clct pattern correction
            if      (clct_pat_sign == 0) eight_str[i] += clct_pat_corr(2,1);
            else if (eight_str[i]  != 0) eight_str[i] -= clct_pat_corr(2,1); // fix on Jia Fu request 2016-11-10
        }

        for (unsigned int i = 0; i < seg_ch; i++) {
            if (vpf[i]) {
                vl_tmp[i] = 1;
                // ph conversion
                // for factors 1024 and 2048 the multiplier should be replaced with shifts by synthesizer
                mult = eight_str[i] * factor[i];
                ph_tmp = mult(mult_bw-1,10);
                ph_init_ix =  me11a[i] ? 2u : 0u; // index of ph_init parameter to apply (different for ME11a and b)

                if (ph_reverse) fph[i] = params[ph_init_ix] - ph_tmp;
                else            fph[i] = params[ph_init_ix] + ph_tmp;

                fph_to_reduce[i] = fph[i] + 16u; // +16 to put the rounded value into the middle of error range
                // divide full ph by 32, subtract chamber start
                ph_reduced[i] = fph_to_reduce[i](bw_fph-1,5) - ph_hard;
                //ph_hit_tmp[ph_reduced[i]] = 1; // set hit in zone
                //ph_hit[ph_reduced[i]] = 1u; // set hit in zone

                wg = wiregroup[i];
                // th conversion
                // call appropriate LUT, it returns th[i] relative to wg0 of that chamber
                th_orig = th_mem[wg];


                // need th duplication here
                for (unsigned int j = 0; j < seg_ch; j++) {
                    if (vpf[j]) {
                        // calculate correction for each strip number
                        // index is: {wiregroup(2 MS bits), dblstrip(5-bit for these chambers)}
                        // rework on Jia Fu request, 2016-11-03
                        // wg_mult = wg * 5'd21;
                        if (me11a[j]) {
                            mult = eight_str[j] * ap_uint<11>(341u); // 341/256 =~ 1.333
                            // index = (wg_mult(9,8), mult(16,12));
                            index = (wg(5,4), mult(16,12));
                        } else {
                            // index = (wg_mult(9,8), eight_str[j](8,4));
                            index = (wg(5,4), eight_str[j](8,4));
                        }

                        th_corr = th_corr_mem[index];

                        // apply correction to the corresponding output
                        th_tmp = (th_orig + th_corr);

                        /* This is a debug variable within prim_conv11
                        th_tmp1[j] = th_tmp;
                        */

                        // check that correction did not make invalid value outside chamber coverage
                        if (wg == 0)              th_tmp = 0; // limit at the bottom
                        if (th_tmp > th_coverage) th_tmp = th_coverage; // limit at the top

                        // apply initial th value for that chamber
                        thf = th_tmp + params[4];
                        if (thf == 0u) thf = 1u;          // protect against invalid value
                        th[j*seg_ch+i] = thf;           // indexes switched i<-->j per Jia Fu request 2016-10-18

                        // check which zones ph hits should be applied to
                        // TODO: Check vs zapp enum and zone_bnd
                        for (int zn = 0; zn < 3; zn++)
                        #pragma HLS unroll
                            if ( // TODO: Check with thf to see if size/any difference
                                th[j*seg_ch+i] >= zns_bnd[1][st][zn][0] &&
                                th[j*seg_ch+i] <= zns_bnd[1][st][zn][1]
                            ) phzvl_tmp[2-zn] = 1; // Bit reversed by Jia Fu request 8/13/21
                    } else {
                        th[j*seg_ch+i] = 0;
                    }
                }
                // Look over
                clctpat_r[i] = clctpat[i]; // just propagate pattern downstream
            } else {
                fph[i] = 0;
                clctpat_r[i] = 0;
                vl_tmp[i] = 0;
            }
            ph[i] = fph[i];
        }
    } else {
        if      (sel == 0) params[addr]      =            r_in;
        else if (sel == 1) th_mem[addr]      = ap_uint<6>(r_in);
        else if (sel == 2) th_corr_mem[addr] = ap_uint<4>(r_in);
    }

    r_out = (sel == 0) ?             params[addr]       :
            (sel == 1) ? ap_uint<13>(th_mem[addr])      :
            (sel == 2) ? ap_uint<13>(th_corr_mem[addr]) :
                         (ap_uint<5>(0), station, cscid);

    if (vl_tmp == 0) { // when none are valid, th[j*seg_ch+i] never gets set to 0 in the else
        th[0] = 0;
        th[1] = 0;
        th[2] = 0;
        th[3] = 0;
    }
    vl = vl_tmp;
    phzvl = phzvl_tmp;

#ifndef __SYNTHESIS__
    for (unsigned int i = 0; i < seg_ch; i++)
        if (!we && vl[i] != 0 && phzvl == 0)
            std::cout << "Warning: PC11 hit not in any zone: st: " << station << " cscid: " << cscid << " th: " << th[i] << std::endl;
    #ifdef PC11_PRINT_MACRO
    for (unsigned int i = 0; i < seg_ch; i++)
        if (!we && vl[i] != 0)
            std::cout << "    st:" << station
                      << "    ch:" << cscid
                      << "    ph:" << ph[i]
                      << "    th:" << th[i]
                      << std::endl;
    #endif // PC_PRINT
#endif // Synthesis
}

#undef th_coverage
//#undef th_negative
