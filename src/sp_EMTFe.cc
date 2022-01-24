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
#include "../interface/sp_EMTFe.h"

// TODO: Is there any cpatr for RPC?
//
// TODO: no need for coord_delay if II=1

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
) {
    using namespace std;

    static coord_delay cdl;

// Keep Nikhils versions for interfaces
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS pipeline II=1
// Check latency later
#pragma HLS latency max=0
#pragma HLS array_partition variable=vpf    dim=0
#pragma HLS array_partition variable=q      dim=0
#pragma HLS array_partition variable=wg     dim=0
#pragma HLS array_partition variable=hstr   dim=0
#pragma HLS array_partition variable=cpat   dim=0
//#pragma HLS array_partition variable=th_corr_mem dim=0
//#pragma HLS array_partition variable=th_mem  dim=0
//#pragma HLS array_partition variable=params  dim=0
#pragma HLS array_partition variable=ph_rpc dim=0
#pragma HLS array_partition variable=th_rpc dim=0
#pragma HLS array_partition variable=time   dim=0
#pragma HLS array_partition variable=bend   dim=0

// Tells Vivado inputs won't change during the work-cycle
// TODO: I dont think I need these here, do this for inputs in wrapper
//       function and protocols will most likely trickle down
#pragma HLS interface ap_none port=vpf
#pragma HLS interface ap_none port=wg
#pragma HLS interface ap_none port=hstr
#pragma HLS interface ap_none port=cpat
//#pragma HLS interface ap_none port=th_corr_mem
//#pragma HLS interface ap_none port=th_mem
//#pragma HLS interface ap_none port=params
#pragma HLS interface ap_none port=ph_rpc
#pragma HLS interface ap_none port=th_rpc
#pragma HLS interface ap_none port=time
#pragma HLS interface ap_none port=bend

#pragma HLS interface ap_none port=endcap
#pragma HLS interface ap_none port=sector
#pragma HLS interface ap_none port=lat_test
#pragma HLS interface ap_none port=core_config

#pragma HLS interface ap_none port=r_in
#pragma HLS interface ap_none port=we
#pragma HLS interface ap_none port=addr
#pragma HLS interface ap_none port=sel
#pragma HLS interface ap_none port=cs
#pragma HLS interface ap_none port=r_out

    //static prim_conv_sector pcs;    // Convert primitives into angular values
    //static zones            zns;    // Construct raw hit zones
    //static extend_sector    exts;   // Extend raw hit pulses
    //static ph_pattern_sector phps;   // Detect patterns

    // coordinates [station][chamber][segment]
#ifndef GOLD_FULL
    ap_uint<bw_fph>  ph   [12][9][seg_ch];
    ap_uint<bw_th>   th11 [3][3][th_ch11];
    ap_uint<bw_th>   th   [12][9][seg_ch];
    ap_uint<seg_ch>  vl   [12][9];
    ap_uint<7>       phzvl[12][9];
    ap_uint<seg_ch>  me11a[3][3];
    ap_uint<4>       cpatr[6][9][seg_ch];
#endif

    //// ph and th raw hits [station][chamber]
    //ap_uint<ph_hit_w> ph_hito [11][9];
    //// ph zones [zone][station]
    //ap_uint<ph_raw_w> ph_zone [7][15];
    //// ph zones extended [zone][station]
    //ap_uint<ph_raw_w> ph_ext  [7][15]; // TODO: previous dimm: [3:0][4:1]
    //// hardcoded (at this time) input
    //ap_uint<3>       drifttime = 2;
//#pragma HLS array_partition variable=ph_hito  dim=0
//#pragma HLS array_partition variable=ph_zone  dim=0
//#pragma HLS array_partition variable=ph_ext   dim=0
#pragma HLS array_partition variable=ph    dim=0
#pragma HLS array_partition variable=th    dim=0
#pragma HLS array_partition variable=th11  dim=0
#pragma HLS array_partition variable=vl    dim=0
#pragma HLS array_partition variable=phzvl dim=0
#pragma HLS array_partition variable=me11a dim=0
#pragma HLS array_partition variable=cpatr dim=0

//    ap_uint<4>  q_e   [12][9][seg_ch];
//    ap_uint<6>  time_e[6][9][seg_ch]; // TODO: Dimmensions recheck, RPC input
//
    //ap_uint<seg_ch>   vl_e    [12][9]; // Currently unused
    //ap_uint<4>        cpatr_e [6][9][seg_ch];  // Currently unused
    //ap_uint<bw_fph>   ph_e    [12][9][seg_ch]; // Currently unused
    //ap_uint<bw_th>    th11_e  [3][3][th_ch11]; // Currently unused
    //ap_uint<bw_th>    th_e    [12][9][seg_ch]; // Currently unused
    //ap_uint<7>        phzvl_e [12][9]; // Currently unused
    //ap_uint<ph_hit_w> ph_hit_e[13][9]; // Unused
    //ap_uint<ph_hit_w> ph_hit  [13][9]; // Unused
    //ap_uint<4>  q_e   [12][9][seg_ch];
    //ap_uint<6>  time_e[6][9][seg_ch]; // TODO: Dimmensions recheck, RPC input
    ap_uint<4>  q_e   [1][12][9][seg_ch];
    ap_uint<6>  time_e[1][12][9][seg_ch]; // TODO: Dimmensions recheck, RPC input
    ap_uint<seg_ch>   vl_e    [1][12][9]; // Currently unused
    ap_uint<4>        cpatr_e [1][6][9][seg_ch];  // Currently unused
    ap_uint<bw_fph>   ph_e    [1][12][9][seg_ch]; // Currently unused
    ap_uint<bw_th>    th11_e  [1][3][3][th_ch11]; // Currently unused
    ap_uint<bw_th>    th_e    [1][12][9][seg_ch]; // Currently unused
    ap_uint<7>        phzvl_e [1][12][9]; // Currently unused
    ap_uint<ph_hit_w> ph_hit_e[1][13][9]; // Unused
    ap_uint<ph_hit_w> ph_hit     [13][9]; // Unused
#pragma HLS array_partition variable=q_e      dim=0
#pragma HLS array_partition variable=time_e   dim=0
#pragma HLS array_partition variable=vl_e     dim=0
#pragma HLS array_partition variable=cpatr_e  dim=0
#pragma HLS array_partition variable=ph_e     dim=0
#pragma HLS array_partition variable=th11_e   dim=0
#pragma HLS array_partition variable=phzvl_e  dim=0
#pragma HLS array_partition variable=ph_hit_e dim=0
#pragma HLS array_partition variable=ph_hit   dim=0

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

    top_in_t in0[TOP_N_IN];
#pragma HLS array_reshape variable=in0 dim=0
//#pragma HLS array_partition variable=in0 dim=0
    // Contains:
    // ap_uint<13> emtf_phi   [230];
    // ap_uint<10> emtf_bend  [230];
    // ap_uint<7>  emtf_theta1[230];
    // ap_uint<7>  emtf_theta2[230];
    // ap_uint<4>  emtf_qual1 [230];
    // ap_uint<4>  emtf_qual2 [230];
    // ap_uint<6>  emtf_time  [230];
    // ap_uint<3>  seg_zones  [230];
    // ap_uint<3>  seg_tzones [230];
    // ap_uint<1>  seg_fr     [230];
    // ap_uint<1>  seg_dl     [230];
    // ap_uint<2>  seg_bx     [230];
    // ap_uint<1>  seg_valid  [230];

    // fold numbers for multiplexed pattern detectors
    ap_uint<3>     ph_foldn = 0;

    // Convert primitives into angular values
    // @160 MHz, 4 clock latency
    prim_conv_sector(
         vpf, wg, hstr, cpat,
         //th_corr_mem, th_mem, params,
         ph_rpc, th_rpc, ph,
         th11, th, vl, phzvl, me11a, cpatr, r_out, endcap, lat_test,
         r_in, we, addr, sel, cs

    );

    // Delay PCS's latency amount to synchronize jia fu HLS's inputs
    //     8-2021, q and time are currently unused in PCS
    //         will need to add extenders when BX>1 and may need to update I/O
    //         for coord_delay when GEMs added and if RPC no longer come
    //         converted to polar coordinates
    cdl(
        // Inputs
        time,
        vl,      // Currently unused
        q,
        cpatr,   // Currently unused
        ph,      // Currently unused
        th11,    // Currently unused
        th,      // Currently unused
        phzvl,   // Currently unused
        ph_hit,  // Unused
        // Outputs
        time_e,
        vl_e,    // Currently unused
        q_e,
        cpatr_e, // Currently unused
        ph_e,    // Currently unused
        th11_e,  // Currently unused
        th_e,    // Currently unused
        phzvl_e, // Currently unused
        ph_hit_e // Currently unused
    );

    // Insert UF HLS -> JF HLS array compatibility layers
    //
    // CSC parts, same as Alex's cscid-to-ring-conversion.pdf for Jia Fu's 115_chambers.py
    for (int ist = 0; ist < 6; ist++) {
        for (int ich = 0; ich < 9; ich++) {
            for (int isg = 0; isg < 2; isg++) {
                #pragma HLS unroll
                int ind = 18*ist + 2*ich + isg;
                in0[ind].emtf_phi   = ph    [ist][ich][isg];
                in0[ind].emtf_bend  = bend  [ist][ich][isg];
                //in0[ind].emtf_qual1 = q_e   [ist][ich][isg];
                in0[ind].emtf_qual1 = q_e[0][ist][ich][isg];
                in0[ind].emtf_qual2 = 0; //cpatr [ist][ich][isg];
                //in0[ind].emtf_time  = time_e[ist][ich][isg];
                in0[ind].emtf_time  = time_e[0][ist][ich][isg];
                //in0[ind].seg_zones  = phzvl [ist][ich][isg];
                in0[ind].seg_zones  = phzvl [ist][ich]; // TODO: phzvl for each seg_ch?
                in0[ind].seg_tzones = 4;
                in0[ind].seg_valid  = vl    [ist][ich][isg]; // Delayed vpf
                in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
                // I asked "Theta1 vs Theta2?"
                // JF says "Each CSC segment has up to 2 possible theta values."
                //     on 4/27/2021
                // But need 217 theta1, so I put in th11[0,2] into theta1,
                //                              and th11[1,3] into theta2 to match pc11 th duplication
                //                              and th[seg] into theta1
                //                              and th[seg] into theta2
                if ((ist < 2 && ich < 3) || (ist == 5 && ich == 0)) {
                    int st = (ist == 5) ? 2 : ist;
                    in0[ind].emtf_theta1 = th11[st][ich][isg*seg_ch]; // TODO use correct th from pc11
                    //in0[ind].emtf_theta2 = th11[st][ich][isg*2+1]; // TODO use correct th from pc11
                    in0[ind].emtf_theta2 = th11[st][ich][isg*seg_ch+1]; // TODO use correct th from pc11
                    //std::cout << "pc11[" << ist << "][" << ich << "] th11[" << isg << "]: " << th11[st][ich][isg] << std::endl
                              //<< "pc11[" << ist << "][" << ich << "] th11[" << isg+2 << "]: " << th11[st][ich][isg+2] << std::endl;
                } else {
                    in0[ind].emtf_theta1 = th  [ist][ich][isg]; // TODO: verify th st 0-3 ich >= 3 isn't offset down to 0
                    in0[ind].emtf_theta2 = th  [ist][ich][isg];
                }
            }
        }
    }

    // RPC parts
    // RPC 1-2/2,3
    // Alex says I can assume these come decoded into their CSC st/ch
    for (int ist = 0; ist < 3; ist++) {
        for (int ich = 3; ich < 9; ich++) {
            for (int isg = 0; isg < 2; isg++) {
                #pragma HLS unroll
                // TODO:  make ph bigger, add in RPC to new PCS with mem inside,
                //        add in ph and th outputs to RPC PC
                //        check if other variables exist for RPC, if not add to
                //        PCS or ask Alex about whether they are inputs
                int ind = 18*(6+ist) + 2*ich + isg;
                in0[ind].emtf_phi    = ph    [6+ist][ich][isg];
                in0[ind].emtf_bend   = bend  [6+ist][ich][isg];
                in0[ind].emtf_theta1 = th    [6+ist][ich][isg];
                in0[ind].emtf_theta2 = th    [6+ist][ich][isg];
                //in0[ind].emtf_qual1  = q_e   [6+ist][ich][isg];
                in0[ind].emtf_qual1  = q_e[0][6+ist][ich][isg];
                in0[ind].emtf_qual2  = 0; //cpatr [ist][ich][isg];
                //in0[ind].emtf_time   = time_e[6+ist][ich][isg];
                in0[ind].emtf_time   = time_e[0][6+ist][ich][isg];
                //in0[ind].seg_zones   = phzvl [6+ist][ich][isg]; // TODO fix
                in0[ind].seg_zones   = phzvl [6+ist][ich]; // TODO fix
                in0[ind].seg_tzones  = 4;
                in0[ind].seg_valid   = vl    [6+ist][ich][isg]; // Delayed vpf
                in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
            }
        }
    }
    // RPC 3,4/1,2
    for (int ist = 3; ist < 5; ist++) {
        for (int ich = 0; ich < 9; ich++) {
            for (int isg = 0; isg < 2; isg++) {
                #pragma HLS unroll
                // TODO:  make ph bigger, add in RPC to new PCS with mem inside,
                //        add in ph and th outputs to RPC PC
                //        check if other variables exist for RPC, if not add to
                //        PCS or ask Alex about whether they are inputs
                int ind = 18*(6+ist) + 2*ich + isg;
                in0[ind].emtf_phi    = ph    [6+ist][ich][isg];
                in0[ind].emtf_bend   = bend  [6+ist][ich][isg];
                in0[ind].emtf_theta1 = th    [6+ist][ich][isg];
                in0[ind].emtf_theta2 = th    [6+ist][ich][isg];
                //in0[ind].emtf_qual1  = q_e   [6+ist][ich][isg];
                in0[ind].emtf_qual1  = q_e[0][6+ist][ich][isg];
                in0[ind].emtf_qual2  = 0; //cpatr [ist][ich][isg];
                //in0[ind].emtf_time   = time_e[6+ist][ich][isg];
                in0[ind].emtf_time   = time_e[0][6+ist][ich][isg];
                //in0[ind].seg_zones   = phzvl [6+ist][ich][isg];
                in0[ind].seg_zones   = phzvl [6+ist][ich];
                in0[ind].seg_tzones  = 4;
                in0[ind].seg_valid   = vl    [6+ist][ich][isg]; // Delayed vpf
                in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
            }
        }
    }
    // RPC neighbors
    // ich 0,3 Reserved for GEM
    // RE 1/2, RE 1/3
    for (int ich = 1; ich < 3; ich++) {
        for (int isg = 0; isg < 2; isg++) {
            #pragma HLS unroll
            // TODO:  may want to unroll myself, so only one value written to ind
            int ind = 18*11 + 2*ich + isg;
            in0[ind].emtf_phi    = ph    [11][ich][isg];
            in0[ind].emtf_bend   = bend  [11][ich][isg];
            in0[ind].emtf_theta1 = th    [11][ich][isg];
            in0[ind].emtf_theta2 = th    [11][ich][isg];
            //in0[ind].emtf_qual1  = q_e   [11][ich][isg];
            in0[ind].emtf_qual1  = q_e[0][11][ich][isg];
            in0[ind].emtf_qual2  = 0; //cpatr [5][ich][isg];
            //in0[ind].emtf_time   = time_e[11][ich][isg];
            in0[ind].emtf_time   = time_e[0][11][ich][isg];
            //in0[ind].seg_zones   = phzvl [11][ich][isg];
            in0[ind].seg_zones   = phzvl [11][ich];
            in0[ind].seg_tzones  = 4;
            in0[ind].seg_valid   = vl    [11][ich][isg]; // Delayed vpf
            in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
        }
    }
    // RE 2/2, RE 3/1,2 and RE 4/1,2
    for (int ich = 4; ich < 9; ich++) {
        for (int isg = 0; isg < 2; isg++) {
            #pragma HLS unroll
            // TODO:  may want to unroll myself, so only one value written to ind
            int ind = 18*11 + 2*ich + isg;
            in0[ind].emtf_phi    = ph    [11][ich][isg];
            in0[ind].emtf_bend   = bend  [11][ich][isg];
            in0[ind].emtf_theta1 = th    [11][ich][isg];
            in0[ind].emtf_theta2 = th    [11][ich][isg];
            //in0[ind].emtf_qual1  = q_e   [11][ich][isg];
            in0[ind].emtf_qual1  = q_e[0][11][ich][isg];
            in0[ind].emtf_qual2  = 0; //cpatr [5][ich][isg];
            //in0[ind].emtf_time   = time_e[11][ich][isg];
            in0[ind].emtf_time   = time_e[0][11][ich][isg];
            //in0[ind].seg_zones   = phzvl [11][ich][isg];
            in0[ind].seg_zones   = phzvl [11][ich];
            in0[ind].seg_tzones  = 4;
            in0[ind].seg_valid   = vl    [11][ich][isg]; // Delayed vpf
            in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
        }
    }

    // GEM parts
    // GE 1,2/1
    for (int ist = 0; ist < 3; ist++) {
        for (int ich = 0; ich < 3; ich++) {
            for (int isg = 0; isg < 2; isg++) {
                #pragma HLS unroll
                // TODO:  make ph bigger, add in RPC to new PCS with mem inside,
                //        add in ph and th outputs to RPC PC
                //        check if other variables exist for RPC, if not add to
                //        PCS or ask Alex about whether they are inputs
                int ind = 18*(6+ist) + 2*ich + isg;
                in0[ind].emtf_phi    = 0; // TODO ph    [6+ist][ich][isg];
                in0[ind].emtf_bend   = 0; // TODO bend  [6+ist][ich][isg];
                in0[ind].emtf_theta1 = 0; // TODO th    [6+ist][ich][isg][0];
                in0[ind].emtf_theta2 = 0; // TODO th    [6+ist][ich][isg][1];
                //in0[ind].emtf_qual1  0; // TODO = q_e   [6+ist][ich][isg];
                in0[ind].emtf_qual1  = 0; // TODO q_e[0][6+ist][ich][isg];
                in0[ind].emtf_qual2  = 0; // TODO = cpatr [6+ist][ich][isg];
                //in0[ind].emtf_time   0; // TODO = time_e[6+ist][ich][isg];
                in0[ind].emtf_time   = 0; // TODO time_e[0][6+ist][ich][isg];
                //in0[ind].seg_zones   0; // TODO = phzvl [6+ist][ich][isg]; // TODO fix
                in0[ind].seg_zones   = 0; // TODO phzvl [6+ist][ich]; // TODO fix
                in0[ind].seg_tzones  = 0; // TODO 4;
                in0[ind].seg_valid   = 0; // vl    [6+ist][ich][isg]; // Delayed vpf
                in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
                in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
            }
        }
    }
    // GEM neighbors
    // GE 1/1 neighbor
    for (int isg = 0; isg < 2; isg++) {
        #pragma HLS unroll
        // TODO:  may want to unroll myself, so only one value written to ind
        int ind = 18*11 + 2*0 + isg;
        in0[ind].emtf_phi    = 0; // TODO ph    [11][0][isg];
        in0[ind].emtf_bend   = 0; // TODO bend  [11][0][isg];
        in0[ind].emtf_theta1 = 0;// TODO th    [11][0][isg][0];
        in0[ind].emtf_theta2 = 0;// TODO th    [11][0][isg][1];
        //in0[ind].emtf_qual1  0;// TODO = q_e   [11][0][isg];
        in0[ind].emtf_qual1  = 0;// TODO q_e[0][11][0][isg];
        in0[ind].emtf_qual2  = 0;// TODO = cpatr [11][0][isg];
        //in0[ind].emtf_time   0;// TODO = time_e[11][0][isg];
        in0[ind].emtf_time   = 0;// TODO time_e[0][11][0][isg];
        //in0[ind].seg_zones   0;// TODO = phzvl [11][0][isg];
        in0[ind].seg_zones   = 0;// TODO phzvl [11][0];
        in0[ind].seg_tzones  = 0;// TODO 4;
        in0[ind].seg_valid   = 0; // vl    [11][0][isg]; // Delayed vpf
        in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
        in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
        in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
    }
    // GE 2/1 neighbor
    for (int isg = 0; isg < 2; isg++) {
        #pragma HLS unroll
        // TODO:  may want to unroll myself, so only one value written to ind
        int ind = 18*11 + 2*3 + isg;
        in0[ind].emtf_phi    = 0; // TODO ph    [11][3][isg];
        in0[ind].emtf_bend   = 0; // TODO bend  [11][3][isg];
        in0[ind].emtf_theta1 = 0; // TODO th    [11][3][isg][0];
        in0[ind].emtf_theta2 = 0; // TODO th    [11][3][isg][1];
        //in0[ind].emtf_qual1  0; // TODO = q_e   [11][3][isg];
        in0[ind].emtf_qual1  = 0; // TODO q_e[0][11][3][isg];
        in0[ind].emtf_qual2  = 0; // TODO = cpatr [11][3][isg];
        //in0[ind].emtf_time   0; // TODO = time_e[11][3][isg];
        in0[ind].emtf_time   = 0; // TODO time_e[0][11][3][isg];
        //in0[ind].seg_zones   0; // TODO = phzvl [11][3][isg];
        in0[ind].seg_zones   = 0; // TODO phzvl [11][3];
        in0[ind].seg_tzones  = 0; // TODO 4;
        in0[ind].seg_valid   = 0; //vl    [11][3][isg]; // Delayed vpf
        in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
        in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
        in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
    }

    // ME 0
//    for (int i = 108; i < 117; i++) {
    for (int i = 108; i < 115; i++) {
        for (int isg = 0; isg < 2; isg++) {
            int ind = i*2 + isg;
            in0[ind].emtf_phi    = 0; // TODO
            in0[ind].emtf_bend   = 0; // TODO
            in0[ind].emtf_theta1 = 0; // TODO
            in0[ind].emtf_theta2 = 0; // TODO
            //in0[ind].emtf_qual1  0; // TODO
            in0[ind].emtf_qual1  = 0; // TODO
            in0[ind].emtf_qual2  = 0; // TODO
            //in0[ind].emtf_time   0; // TODO
            in0[ind].emtf_time   = 0; // TODO
            //in0[ind].seg_zones   0; // TODO
            in0[ind].seg_zones   = 0; // TODO
            in0[ind].seg_tzones  = 0; // TODO
            in0[ind].seg_valid   = 0; // TODO
            in0[ind].seg_fr = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_dl = 0; // TODO: currently unused, so zero'd
            in0[ind].seg_bx = 0; // TODO: currently unused, so zero'd
        }
    }
    if (we)
        return;
    /*
    #ifndef __SYNTHESIS__
    std::cout << "emtf_phi:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_phi;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_theta1:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_theta1;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_theta2:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_theta2;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_bend:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_bend;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_qual1:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_qual1;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_qual2:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_qual2;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "emtf_time:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].emtf_time;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "seg_zones:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].seg_zones;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "seg_valid:" << std::endl;
    for (int i = 0; i < TOP_N_IN; i++) {
        std::cout << in0[i].seg_valid;
        if (i < TOP_N_IN-1)
            std::cout << " ";
    }
    std::cout << std::endl;
    #endif
    */
//    // TODO: verify index mapping, been too long since I inferred them from
//    //       coord_delay.sv
//    for (int isb = 1; isb < 7; isb++) { // 0 are neighbors
//        for (int ich = 0; ich < 8; ich++) {
//            // From prim_conv_sector, RPC -> CSC definition
//            ap_uint<4> station = (ich == 0 && isb > 3) ? 1 :
//                                 (ich == 0)            ? 0 :
//                                 (ich == 1)            ? 2 :
//                                 (ich == 2 || ich == 3 || ich == 6)? 3 : // 6 is iRPC3/1
//                                 (ich == 4 || ich == 5 || ich == 7)? 4 : // 7 is iRPC4/1
//                                                                    -1;
//            ap_uint<4> rpcid   = (ich == 0 && isb > 3) ? isb - 1 :
//                                                         isb + 2; // Subsector 1 aligns with chamber 3 in non-ME11 stations
//            for (int isg = 0; isg < 2; isg++) {
//                // TODO:  make ph bigger, add in RPC to new PCS with mem inside,
//                //        add in ph and th outputs to RPC PC
//                //        check if other variables exist for RPC, if not add to
//                //        PCS or ask Alex about whether they are inputs
//                int ind = 18*(6+station) + 2*rpcid + isg; // 6 accounts for 6 CSC stations
//                in0[ind].emtf_phi    = ph    [6+isb][ich][isg];
//                in0[ind].emtf_bend   = bend  [6+isb][ich][isg];
//                in0[ind].emtf_theta1 = th    [6+isb][ich][isg][0];
//                in0[ind].emtf_theta2 = th    [6+isb][ich][isg][1];
//                in0[ind].emtf_qual1  = q_e   [6+isb][ich][isg];
//                in0[ind].emtf_qual2  = cpatr [6+isb][ich][isg];
//                in0[ind].emtf_time   = time_e[6+isb][ich][isg];
//                in0[ind].seg_zones   = phzvl [6+isb][ich][isg]; // TODO fix
//                in0[ind].seg_valid   = vl    [6+isb][ich][isg]; // Delayed vpf
//            }
//        }
//    }
//    // RPC neighbors
//    // TODO: verify index mapping, been too long since I inferred them from
//    //       coord_delay.sv
//    for (int ich = 0; ich < 8; ich++) {
//        // From prim_conv_sector, RPC -> CSC definition
//        ap_uint<4> station = (j == 0) ? 1 :
//                             (j == 1) ? 2 :
//                             (j < 4 || j == 6)? 3 :
//                             (j < 6 || j == 7)? 4 :
//                                               -1;
//        ap_uint<4> rpcid   = (j == 0) ? 5 :
//                             (j == 1) ? 8 :
//                             (j < 4 || j == 6)? 8 :
//                             (j < 6 || j == 7)? 8 :
//                                               -1;
//        for (int isg = 0; isg < 2; isg++) {
//            // TODO:  may want to unroll myself, so only one value written to
//            //        ind
//            int ind = 18*(10+station) + 2*(rpcid/3) + isg; // 10 accounts for 6 CSC stations and 4 RPC stations, rpc/3 because only one index per row of 3
//            in0[ind].emtf_phi    = ph    [6][ich][isg];
//            in0[ind].emtf_bend   = bend  [6][ich][isg];
//            in0[ind].emtf_theta1 = th    [6][ich][isg][0];
//            in0[ind].emtf_theta2 = th    [6][ich][isg][1];
//            in0[ind].emtf_qual1  = q_e   [6][ich][isg];
//            in0[ind].emtf_qual2  = cpatr [6][ich][isg];
//            in0[ind].emtf_time   = time_e[6][ich][isg];
//            in0[ind].seg_zones   = phzvl [6][ich][isg]; // TODO fix
//            in0[ind].seg_valid   = vl    [6][ich][isg]; // Delayed vpf
//        }
//    }

    // Insert JF HLS function call
    myproject(in0, model_out);
}

