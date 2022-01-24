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
#include <ap_int.h>
#include "../interface/spbits.h"
#include "../interface/prim_conv_sector_EMTFe.h"
#include "../interface/zones_EMTFe.h"
#include "../interface/extend_sector_EMTFe.h"
#include "../interface/ph_pattern_sector_EMTFe.h"

void pze_tl(
    // [station][chamber][segment] station 5 = neighbor sector, all stations
    // unpack LCTs in Verilog code -- unable to do so here without increasing II
    const ap_uint<seg_ch>   vpf      [13][9],
    const ap_uint<bw_wg>    wg       [6][9][seg_ch],
    const ap_uint<bw_hs>    hstr     [6][9][seg_ch],
    const ap_uint<4>        cpat     [6][9][seg_ch],
    //const ap_uint<64>       cppf_rxd [7][3], // cppf rx data, 3 frames x 64 bit, for 7 links
    //const ap_uint<7>        cppf_rx_valid,   // cprx data valid flags
    const ap_uint<4>        th_corr_mem[3][3][th_corr_mem_sz],
    const ap_uint<6>        th_mem     [6][9][th_mem_sz],
    const ap_uint<13>       params     [6][9][6],
    const ap_uint<bw_fph-2> ph_rpc[7][8][seg_ch],
    const ap_uint<bw_th-2>  th_rpc[7][8][seg_ch],

    ap_uint<11>     ph_rank [7][ph_raw_w],

    const ap_uint<1>       &endcap,
    const ap_uint<3>       &sector,
    const ap_uint<1>       &lat_test,
    const ap_uint<64>      &core_config
) {
    using namespace std;

// Keep Nikhils versions for interfaces
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS array_partition variable=vpf     dim=0
#pragma HLS array_partition variable=wg      dim=0
#pragma HLS array_partition variable=hstr    dim=0
#pragma HLS array_partition variable=cpat    dim=0
#pragma HLS array_partition variable=th_corr_mem dim=0
#pragma HLS array_partition variable=th_mem  dim=0
#pragma HLS array_partition variable=params  dim=0
#pragma HLS array_partition variable=ph_rpc  dim=0
#pragma HLS array_partition variable=th_rpc  dim=0

// Tells Vivado inputs won't change during the work-cycle
// TODO: I dont think I need these here, do this for inputs in wrapper
//       function and protocols will most likely trickle down
/*
#pragma HLS INTERFACE ap_stable port=q
#pragma HLS INTERFACE ap_stable port=hstr
#pragma HLS INTERFACE ap_stable port=cpat
#pragma HLS INTERFACE ap_stable port=wg
#pragma HLS INTERFACE ap_stable port=pcs_cs
#pragma HLS INTERFACE ap_stable port=endcap
#pragma HLS INTERFACE ap_stable port=lat_test
#pragma HLS INTERFACE ap_stable port=sector
*/

    static prim_conv_sector pcs;    // Convert primitives into angular values
    static zones            zns;    // Construct raw hit zones
    static extend_sector    exts;   // Extend raw hit pulses
    static ph_pattern_sector phps;   // Detect patterns

    // coordinates [station][chamber][segment]
    ap_uint<bw_fph>  ph    [6][9][seg_ch];
    ap_uint<bw_th>   th11  [3][3][th_ch11];
    ap_uint<bw_th>   th    [6][9][seg_ch];
    ap_uint<seg_ch>  vl    [6][9];
    ap_uint<7>       phzvl [13][9];
    ap_uint<seg_ch>  me11a [3][3];
    ap_uint<4>       cpatr [6][9][seg_ch];
    // ph and th raw hits [station][chamber]
    ap_uint<ph_hit_w> ph_hito [13][9];
    // ph zones [zone][station]
    ap_uint<ph_raw_w> ph_zone [7][15];
    // ph zones extended [zone][station]
    ap_uint<ph_raw_w> ph_ext  [7][15]; // TODO: dimm: [3:0][4:1]
    // hardcoded (at this time) input
    ap_uint<3>       drifttime = 2;
#pragma HLS array_partition variable=ph_hito  dim=0
#pragma HLS array_partition variable=ph_zone  dim=0
#pragma HLS array_partition variable=ph_ext   dim=0
#pragma HLS array_partition variable=ph    dim=0
#pragma HLS array_partition variable=th    dim=0
#pragma HLS array_partition variable=th11  dim=0
#pragma HLS array_partition variable=vl    dim=0
#pragma HLS array_partition variable=phzvl dim=0
#pragma HLS array_partition variable=me11a dim=0
#pragma HLS array_partition variable=cpatr dim=0


    // numbers of best ranks [zone][num]
    /*(* mark_debug = "FALSE" *)*/
    //ap_uint<bpow+1>  ph_num [4][3];
    // best ranks [zone][num]
    /*(* mark_debug = "FALSE" *)*/
    //ap_uint<bwr>     ph_q  [4][3];
    //ap_uint<bwr>     ph_qr [4][3];


    // split config register into parameters
    const ap_uint<bw_th> &th_window      = core_config(bw_th-1,0);
    const ap_uint<4>     &delay_two_mu   = core_config(11,8);
    const ap_uint<1>     &en_single      = core_config[12];
    const ap_uint<1>     &en_two_mu      = core_config[13];
    const ap_uint<1>     &low_th_promote = core_config[14];
    const ap_uint<1>     &use_rpc        = core_config[15];
    const ap_uint<bw_th> &th_window_z0   = core_config(22,16);
    const ap_uint<1>     &two_st_tight_timing = core_config[23];

    // fold numbers for multiplexed pattern detectors
    ap_uint<3>     ph_foldn = 0;

    // coordinate outputs delayed and with history [bx_history][station][chamber][segment]
    // most recent in bx = 0
//    ap_uint<bw_fph>   phd   [max_drift][6][9][seg_ch];
//    ap_uint<bw_th>    th11d [max_drift][3][3][th_ch11];
//    ap_uint<bw_th>    thd   [max_drift][6][9][seg_ch];
//    ap_uint<seg_ch>   vld   [max_drift][6][9];
//    ap_uint<seg_ch>   me11ad[max_drift][3][3];
//    ap_uint<4>        cpatd [max_drift][6][9][seg_ch];

    //ap_uint<seg_ch>   vld_w   [6][9];
    //ap_uint<4>        cpatd_w [6][9][seg_ch];
    //assign vld_w = vld[0];
    //assign cpatd_w = cpatd[0];

//    // find_segment outputs, in terms of segments match in zones [zone][pattern_num][station 0-3]
//    ap_uint<seg_ch> patt_ph_vi [4][3][4];         // valid
//    ap_uint<2>      patt_ph_hi [4][3][4];         // bx index
//    ap_uint<3>      patt_ph_ci [4][3][4];         // chamber
//    ap_uint<4>      patt_ph_si [4][3];            // segment
//    ap_uint<bw_fph> ph_match   [4][3][4];         // matching ph
//    ap_uint<bw_th>  th_match   [4][3][4][seg_ch]; // matching th, 2 segments
//    ap_uint<4>      cpat_match [4][3][4];         // matching pattern
//
//    ap_uint<bw_fph> phi     [4][3];
//    ap_uint<bw_th>  theta   [4][3];
//    ap_uint<4>      cpattern[4][3][4];
//    // ph and th deltas from best stations
//    // [zone][pattern_num], last index: 0=12, 1=13, 2=14, 3=23, 4=24, 5=34
//    ap_uint<bw_fph> delta_ph[4][3][6];
//    ap_uint<bw_th>  delta_th[4][3][6];
//    ap_uint<6>      sign_ph [4][3];
//    ap_uint<6>      sign_th [4][3];
//    // updated ranks [zone][pattern_num]
//    ap_uint<bwr+1>  rank[4][3];
//    //[zone][pattern_num][station 0-3]
//    ap_uint<seg_ch> vir [4][3][4]; // valid
//    ap_uint<2>      hir [4][3][4]; // bx index
//    ap_uint<3>      cir [4][3][4]; // chamber
//    ap_uint<4>      sir [4][3];    // segment
//
//    ap_uint<2>      vl_stub;
//    ap_uint<bw_fph> ph_stub  [2];
//    ap_uint<bw_th>  th_stub  [2];
//    ap_uint<4>      cpat_stub;
//    ap_uint<bwr+1>  bt_rank_i[3];


    {
    #pragma HLS pipeline II=1
        // convert primitives into angular values
        pcs(
             vpf, wg, hstr, cpat, th_corr_mem, th_mem, params, ph_rpc, th_rpc, ph,
             th11, th, vl, phzvl, me11a, cpatr, ph_hito, endcap, lat_test
        );

        // Parentheses are this module's, .xxx is named module's
        // construct raw hit zones
        zns(phzvl, ph_hito, ph_zone);

        // extend raw hit pulses
        exts(ph_zone, ph_ext, drifttime);

        // detect ph patterns in all zones
        phps(ph_ext, drifttime, ph_foldn, ph_rank);
    }

    /*
    // find 3 best ph patterns in each zone
    srts(ph_rank, ph_num, ph_q);

    // delay line for polar coordinates provided by prim_conv_sector
    cdl(
        ph, th11, th, vl, me11a, cpatr, cppf_rxd, cppf_rx_valid, use_rpc, phd,
        th11d, thd, vld, me11ad, cpatd
    );


    // match ph patterns to segments
    // reroute segments according to ph zones
    mphseg(
        ph_num, ph_q, phd, vld, th11d, thd, cpatd, patt_ph_vi, patt_ph_hi,
        patt_ph_ci, patt_ph_si, ph_match, th_match, cpat_match, ph_q
    );

    // delta ph and th for 12 candidates
    ds(
        patt_ph_vi, patt_ph_hi, patt_ph_ci, patt_ph_si, ph_match, th_match,
        cpat_match, ph_qr, th_window, th_window_z0, two_st_tight_timing, phi,
        theta, cpattern, delta_ph, delta_th, sign_ph, sign_th, rank, vir, hir,
        cir, si
    );


    // identify best three tracks
    bt(
        phi, theta, cpattern, delta_ph, delta_th, sign_ph, sign_th, rank, vir,
        hir, cir, sir, bt_phi, bt_theta, bt_cpattern, bt_delta_ph, bt_delta_th,
        bt_sign_ph, bt_sign_th, bt_rank_i, bt_vi, bt_hi, bt_ci, bt_s
    );

    // single hit trigger
    // take only chambers from this sector, not neighbor
    sngl(
        ph(4,0), th11(1,0), th(4,0), vl(4,0), vl_stub[0], ph_stub[0],
        th_stub[0], en_singl
    );

    // two-muon trigger, one track + one single hit in ME1/1
    twom(
        ph, th11, th, vl, me11a, cpatr, bt_rank_i, bt_vi, bt_hi, bt_ci, bt_si,
        vl_stub[1], ph_stub[1], th_stub[1], cpat_stub, delay_two_mu, en_two_mu
    );

    // construct PT LUT addresses for best tracks
    pta(
        bt_phi, bt_theta, bt_cpattern, bt_delta_ph, bt_delta_th, bt_sign_ph,
        bt_sign_th, bt_rank_i, bt_vi, bt_hi, bt_ci, bt_si,
        vl_stub[0] | vl_stub[1], ph_stub[0] | ph_stub[1],
        th_stub[0] | th_stub[1], cpat_stub, ptlut_addr, ptlut_cs,
        ptlut_addr_val, bt_rank, gmt_phi, gmt_eta, gmt_qlt, gmt_crg, sector,
        endcap, low_th_promote
    );
    */

}

