#include "../interface/ph_pattern_EMTFe.h"

void php_tl(
    const ap_uint<window_size> st0,  // ME11
    const ap_uint<window_size> st1,  // ME12
    const ap_uint<1>           st2,  // ME2
    const ap_uint<window_size> st3,  // ME3
    const ap_uint<window_size> st4,  // ME4
    const ap_uint<window_size> st5,  // RE1
    const ap_uint<window_size> st6,  // RE2
    const ap_uint<window_size> st7,  // RE3
    const ap_uint<window_size> st8,  // RE4
    const ap_uint<window_size> st9,  // GE11
    const ap_uint<window_size> st10, // GE21
    const ap_uint<window_size> st11, // ME0
    const ap_uint<window_size> st12, // MB1
    const ap_uint<window_size> st13, // MB2
    const ap_uint<window_size> st14, // MB3
    const ap_uint<window_size> st15, // MB4
    const ap_uint<3>           drifttime,
    const ap_uint<3>           foldn,
    ap_uint<11>                qcode
) {
#pragma HLS pipeline II=1
#pragma HLS latency max=1
    static ph_pattern php;
    php(
        st0, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10, st11, st12,
        st13, st14, st15, drifttime, foldn, qcode
    );
}

