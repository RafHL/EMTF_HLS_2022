#include <ap_int.h>
#include "../interface/spbits.h"
#include "../interface/prim_conv_sector_EMTFe.h"

void pcs_tl(
    // lct parameters [station][chamber][segment]
    // st 5 = neighbor sector, all stations
    const ap_uint<seg_ch>    vpf   [6][9],              /* input  */
    const ap_uint<bw_wg>     wg    [6][9][seg_ch],       /* input  */
    const ap_uint<bw_hs>     hstr  [6][9][seg_ch],       /* input  */
    const ap_uint<4>         cpat  [6][9][seg_ch],       /* input  */
    const ap_uint<4>         th_corr_mem[3][3][th_corr_mem_sz], /* input */
    const ap_uint<6>         th_mem[6][9][th_mem_sz],    /* input  */
    const ap_uint<13>        params[6][9][6],            /* input  */
          ap_uint<bw_fph>    ph    [6][9][seg_ch],       /* output */
    // special th outputs for ME11 because of duplication
    // [station][chamber][segment with duplicates], station 2 = neighbor segment
          ap_uint<bw_th>     th11  [3][3][th_ch11],      /* output */
          ap_uint<bw_th>     th    [6][9][seg_ch],       /* output */
          ap_uint<seg_ch>    vl    [6][9],               /* output */
          ap_uint<7>         phzvl [6][9],              /* output */
    // me11a flags only for ME11 (stations 1,0, chambers 2:0)
    // [station][chamber][segment], station 2 = neighbor segment
          ap_uint<seg_ch>    me11a [3][3],               /* output */
          ap_uint<4>         cpatr [6][9][seg_ch],       /* output */
    // ph and th raw hits
          //TODO: Fix first index once I am sure about the RPC geometry
          ap_uint<ph_hit_w>  ph_hit[6][9],              /* output */
    const ap_uint<1>        &endcap,                     /* input  */
    const ap_uint<1>        &lat_test                    /* input  */
) {
#pragma HLS pipeline II=1
#pragma HLS latency max=0
#pragma HLS interface ap_ctrl_none port=return

#pragma HLS array_partition variable=vpf
#pragma HLS array_partition variable=wg
#pragma HLS array_partition variable=hstr
#pragma HLS array_partition variable=cpat
#pragma HLS array_partition variable=th_corr_mem
#pragma HLS array_partition variable=th_mem
#pragma HLS array_partition variable=params
#pragma HLS array_partition variable=ph
#pragma HLS array_partition variable=th11
#pragma HLS array_partition variable=th
#pragma HLS array_partition variable=vl
#pragma HLS array_partition variable=phzvl
#pragma HLS array_partition variable=me11a
#pragma HLS array_partition variable=cpatr
#pragma HLS array_partition variable=ph_hit

#pragma HLS interface ap_none port=vpf
#pragma HLS interface ap_none port=wg
#pragma HLS interface ap_none port=hstr
#pragma HLS interface ap_none port=cpat
#pragma HLS interface ap_none port=th_corr_mem
#pragma HLS interface ap_none port=th_mem
#pragma HLS interface ap_none port=params
#pragma HLS interface ap_none port=ph
#pragma HLS interface ap_none port=th11
#pragma HLS interface ap_none port=th
#pragma HLS interface ap_none port=vl
#pragma HLS interface ap_none port=phzvl
#pragma HLS interface ap_none port=me11a
#pragma HLS interface ap_none port=cpatr
#pragma HLS interface ap_none port=ph_hit
#pragma HLS interface ap_none port=endcap
#pragma HLS interface ap_none port=lat_test

    static prim_conv_sector pcs;

    pcs(
        vpf, wg, hstr, cpat, th_corr_mem, th_mem, params, ph,
        th11, th, vl, phzvl, me11a, cpatr, ph_hit, endcap, lat_test
    );
}
