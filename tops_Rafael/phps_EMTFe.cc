#include "../interface/ph_pattern_sector_EMTFe.h"

void phps_tl(
    const ap_uint<ph_raw_w>  st[7][15],
    const ap_uint<3>        &drifttime,
    const ap_uint<3>        &foldn,
          ap_uint<11>        qcode[7][ph_raw_w]
) {
#pragma HLS pipeline

#pragma HLS array_partition variable=st        dim=0
#pragma HLS array_partition variable=qcode     dim=0

    static ph_pattern_sector phps;
    phps(
        st, drifttime, foldn, qcode
    );
}

