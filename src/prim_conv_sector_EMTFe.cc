//////////////////////////////////////////////////////////////////////////////////
// Company: UF
// Author: Rafael Hernandez
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
#include "../interface/prim_conv_sector_EMTFe.h"

void prim_conv_sector( // TODO: Check RPC vs Jia Fu's file
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
) {
#pragma HLS protocol fixed

#pragma HLS interface ap_ctrl_none port=return
//#pragma HLS inline off
#pragma HLS pipeline

// Look into largest blocks mainting II of 1
// TODO: Probably needed at first input
#pragma HLS array_partition variable=vpf    dim=0
#pragma HLS array_partition variable=wg     dim=0
#pragma HLS array_partition variable=hstr   dim=0
#pragma HLS array_partition variable=cpat   dim=0
#pragma HLS array_partition variable=ph_rpc dim=0
#pragma HLS array_partition variable=th_rpc dim=0
#pragma HLS array_partition variable=ph     dim=0
#pragma HLS array_partition variable=th11   dim=0
#pragma HLS array_partition variable=th     dim=0
#pragma HLS array_partition variable=vl     dim=0
#pragma HLS array_partition variable=phzvl  dim=0
#pragma HLS array_partition variable=me11a  dim=0
#pragma HLS array_partition variable=cpatr  dim=0
//#pragma HLS array_partition variable=ph_hit dim=0
#pragma HLS array_partition variable=cs      dim=0

    ap_uint<seg_ch> dummy  [6][9]; // me11a for pc variables which always send 0
    ap_uint<9>      we_w   [6];
    //~35 min w/o rout_w
    ap_uint<13>     r_out_w[6][9];
    ap_uint<13>     r_out_tmp = 0;
#pragma HLS array_partition variable=dummy   dim=0
#pragma HLS array_partition variable=we_w    dim=0
#pragma HLS array_partition variable=r_out_w dim=0

    static ap_uint<4>      th_corr_mem[3][3][th_corr_mem_sz];
    static ap_uint<6>      th_mem[6][9][th_mem_sz];
    static ap_uint<13>     params[6][9][6];
#pragma HLS array_partition variable=th_corr_mem dim=0
#pragma HLS array_partition variable=th_mem      dim=0
#pragma HLS array_partition variable=params      dim=0

    // Initialize nontrivial neighbor station/chamber
        /*
        for (ap_uint<4> j = 0; j < 9; j++) {
            // 5 is neighbors:
            // 0: NC
            // 1: RE1/2
            // 2: RE1/3
            // 3: NC
            // 4: RE2/2
            // 5: RE3/1
            // 6: RE3/2
            // 7: RE4/1
            // 8: RE4/2
            strn[j] = (j < 3) ? 1 :
                      (j < 5) ? 2 :
                      (j < 7) ? 3 :
                      (j < 9) ? 4 :
                               -1;
            chrn[j] = (j == 0 || j == 3) ? -1 : // Reserved for GE by JF
                                (j == 1) ? 2 :
                                (j == 2) ? 3 :
                                (j == 4) ? 2 :
                                (j == 5) ? 1 :
                                (j == 6) ? 2 :
                                (j == 7) ? 1 :
                                (j == 8) ? 2 :
                                          -1;
    */
    const ap_uint<4> strn[9] = {  // rpc  station/chamber
        -1, 1, 1, -1, 2, 3, 3, 4, 4
    };
    const ap_uint<4> chrn[9] = {
        -1, 2, 3, -1, 2, 1, 2, 1, 2
    };
    /*
    cscn11_st_ch:
        stn[0] = 0;
        chn[0] = 12;
        // block: cscn11_st_ch

        // rest of neighbor chambers
    cscn_st_ch:
        for (ap_uint<4> j = 1; j < 9; j++) {
            stn[j] = j < 3 ? ap_uint<4>(0)      : ap_uint<4>((j+1)/2);
            chn[j] = j < 3 ? ap_uint<4>(j + 12) : ap_uint<4>((j-1)%2 + 9);
        } // block: cscn_st_ch
    //block: genblk_st_ch
    */
    const ap_uint<4> stn[9] = {     // pcn11 @0, pcn @1-8 station/chamber
        0, 0, 0, 2, 2, 3, 3, 4, 4
    };
    const ap_uint<4> chn[9] = {
        12, 13, 14, 9, 10, 9, 10, 9, 10
    };

#pragma HLS array_partition variable=strn    dim=0
#pragma HLS array_partition variable=chrn    dim=0
#pragma HLS array_partition variable=stn     dim=0
#pragma HLS array_partition variable=chn     dim=0

    for (ap_uint<4> i = 0; i < 6; i++)
#pragma HLS UNROLL
        we_w[i] = (we) ? cs[i] : ap_uint<9>(0);
    // genblk:

// TODO: add in ZAPP
station11:
    for (ap_uint<4> i = 0; i < 2; i++) // FIXME: pc11 index out of range at 1.0
    csc11:
        for (ap_uint<4> j = 0; j < 3; j++)
            csc_prim_conv11( // pc11
                vpf         [i][j],
                wg          [i][j],
                hstr        [i][j],
                cpat        [i][j],
                th_corr_mem [i][j],
                th_mem      [i][j],
                params      [i][j],
                i, // st
                j, // ch
                r_in,
                we_w        [i][j],
                addr,
                sel,
                ph          [i][j],
                th11        [i][j], // use special th11 array for ME11
                vl          [i][j],
                phzvl       [i][j],
                me11a       [i][j],
                cpatr       [i][j],
                //ph_hit      [i][j],
                r_out_w[i][j],
                endcap
            );
    // block: station11

station12:
    for (ap_uint<4> i = 0; i < 2; i++)
    csc12:
        for (ap_uint<4> j = 3; j < 9; j++)
            // XXX: Why no latency test?
            csc_prim_conv( // pc12
                vpf    [i][j],
                wg     [i][j],
                hstr   [i][j],
                cpat   [i][j],
                th_mem [i][j],
                params [i][j],
                i, // st
                j, // ch
                r_in,
                we_w   [i][j],
                addr,
                sel,
                ph     [i][j],
                th     [i][j],
                vl     [i][j],
                phzvl  [i][j],
                dummy  [i][j],
                cpatr  [i][j],
                //ph_hit [i][j],
                r_out_w[i][j],
                endcap,
                0              // Disconnected in verilog code
            );
    // block: station12

station:
    for (ap_uint<4> i = 2; i < 5; i++)
    csc:
        for (ap_uint<4> j = 0; j < 9; j++)
            csc_prim_conv( // pc
                vpf    [i][j],
                wg     [i][j],
                hstr   [i][j],
                cpat   [i][j],
                th_mem [i][j],
                params [i][j],
                i, // st
                j, // ch
                r_in,
                we_w   [i][j],
                addr,
                sel,
                ph     [i][j],
                th     [i][j],
                vl     [i][j],
                phzvl  [i][j],
                dummy  [i][j],
                cpatr  [i][j],
                //ph_hit [i][j],
                r_out_w[i][j],
                endcap,
                lat_test
            );
    // block: station

station_r:
    for (ap_uint<4> i = 0; i < 5; i++)
    rpc:
        for (ap_uint<4> j = 0; j < 9; j++) // TODO: add in vl,th
            rpc_prim_conv( // pcr
                ph_rpc[i][j],
                th_rpc[i][j],
                i,
                j,
                vpf   [6+i][j],
                ph    [6+i][j],
                th    [6+i][j],
                vl    [6+i][j],
                phzvl [6+i][j],
                //ph_hit[6+i][j],
                lat_test
            );
    // Neighbors in i = 5
    for (ap_uint<4> j = 0; j < 9; j++) // TODO: add in vl,th
        rpc_prim_conv( // pcr
            ph_rpc[5][j],
            th_rpc[5][j],
            //stn  [j],
            //chn  [j],
            strn  [j],
            chrn  [j],
            vpf   [11][j],
            ph    [11][j],
            th    [11][j],
            vl    [11][j],
            phzvl [11][j],
            //ph_hit[6+i][j],
            lat_test
        );
    // block: station_r

    // neighbor sector single ME11 chamber
cscn11:
    csc_prim_conv11( // pcn11       // st0, ch12, see ind for variables
        vpf         [5][0],
        wg          [5][0],
        hstr        [5][0],
        cpat        [5][0],
        th_corr_mem [2][0],
        th_mem      [5][0],
        params      [5][0],
        stn            [0],
        chn            [0],
        r_in,
        we_w        [5][0],
        addr,
        sel,
        ph          [5][0],
        th11        [2][0], // use special th11 array for ME11
        vl          [5][0],
        phzvl       [5][0],
        me11a       [2][0],
        cpatr       [5][0],
        //ph_hit      [5][0],
        r_out_w     [5][0],
        endcap
    );
    // block: cscn11

    // rest of neighbor chambers
cscn:
    for (ap_uint<4> j = 1; j < 9; j++)
        csc_prim_conv( // pcn
            vpf    [5][j],
            wg     [5][j],
            hstr   [5][j],
            cpat   [5][j],
            th_mem [5][j],
            params [5][j],
            stn       [j],
            chn       [j],
            r_in,
            we_w   [5][j],
            addr,
            sel,
            ph     [5][j],
            th     [5][j],
            vl     [5][j],
            phzvl  [5][j],
            dummy  [5][j],
            cpatr  [5][j],
            //ph_hit [5][j],
            r_out_w[5][j],
            endcap,
            lat_test
        );
    // block: cscn
// block: genblk

    // mux LUT outputs to r_out
    for (int s = 0; s < 6; s++) { // station loop
        for (int c = 0; c < 9; c++) { // chamber loop
            if (cs[s][c]) {
                r_out_tmp = r_out_tmp | r_out_w[s][c];
            }
        }
    }
    r_out = r_out_tmp;
}

