#ifdef _LOCAL_AP_INT_H_
  #include "../include/ap_int.h"
#else
  #include <ap_int.h>
#endif
#include "../interface/sp_EMTFe.h"

void sp_tl(
        // [station][chamber][segment] station 5 = neighbor sector, all stations
        // unpack LCTs in Verilog code -- unable to do so here without increasing II
        const ap_uint<seg_ch>   vpf      [12][9], // 6 CSC, 5 RPC
        const ap_uint<4>        q        [12][9][seg_ch],
        const ap_uint<bw_wg>    wg       [6][9][seg_ch],
        const ap_uint<bw_hs>    hstr     [6][9][seg_ch],
        const ap_uint<4>        cpat     [6][9][seg_ch],
        const ap_uint<4>        th_corr_mem[3][3][th_corr_mem_sz],
        const ap_uint<6>        th_mem     [6][9][th_mem_sz],
        const ap_uint<13>       params     [6][9][6],
        // Alex says I can assume RPC <> CSC already comes in decoded
        const ap_uint<bw_fph-2> ph_rpc[6][9][seg_ch],
        const ap_uint<bw_th-2>  th_rpc[6][9][seg_ch],
        const ap_uint<6>        time  [6][9][seg_ch], // TODO: Dimmensions recheck, RPC input
        const ap_uint<10>       bend  [12][9][seg_ch],

              ap_int<14>        model_out[160],

        const ap_uint<1>       &endcap,
        const ap_uint<3>       &sector,
        const ap_uint<1>       &lat_test,
        const ap_uint<64>       core_config
) {
    #pragma HLS pipeline
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

    #pragma HLS INTERFACE ap_none port=vpf
    #pragma HLS INTERFACE ap_none port=wg
    #pragma HLS INTERFACE ap_none port=hstr
    #pragma HLS INTERFACE ap_none port=cpat
    #pragma HLS INTERFACE ap_none port=th_corr_mem
    #pragma HLS INTERFACE ap_none port=th_mem
    #pragma HLS INTERFACE ap_none port=params
    #pragma HLS INTERFACE ap_none port=ph_rpc
    #pragma HLS INTERFACE ap_none port=th_rpc

    #pragma HLS INTERFACE ap_none port=endcap
    #pragma HLS INTERFACE ap_none port=sector
    #pragma HLS INTERFACE ap_none port=lat_test
    #pragma HLS INTERFACE ap_none port=core_config

    static sp sp_o;
    sp_o(
        vpf, q, wg, hstr, cpat, th_corr_mem, th_mem, params, ph_rpc, th_rpc,
        time, bend, model_out, endcap, sector, lat_test, core_config
        //vpf, q, wg, hstr, cpat, th_corr_mem, th_mem, params, ph_rpc, th_rpc,
        //time, bend,
        //model_out,
        //endcap, sector, lat_test, core_config
    );
}

