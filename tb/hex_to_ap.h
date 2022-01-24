#ifndef _HEX_TO_AP_H_
#define _HEX_TO_AP_H_

#include <string>

// include this after ap_int.h

char nib_to_dec(char a) {
    a |= 0x20;  // ignore caps
    if (a >= 'a') return 10 + a - 'a';
    else          return a - '0';
}

template <unsigned int SIZE>
ap_uint<SIZE> hex_to_apu(const std::string &str) {
// accepts strings formated as 0xffff, ffff, with no importance on case (a or a)
    ap_uint<SIZE> ap_val = 0;
    size_t here = str.size();
    size_t low = 0;
    size_t ind = 0;

    if (!here)          // empty
        return ap_uint<SIZE>(0);

    if (str[0] == '0' && (0x20|str[1]) == 'x') // takes care of 0x or 0X
        low = 2;

    while (--here > low) {
        ap_val(3+ind, ind) = nib_to_dec(str[here]);
        ind += 4;
    }
    ap_val(SIZE-1, ind) = nib_to_dec(str[here]);
    return ap_val;
}

template <unsigned int SIZE>
ap_int<SIZE> hex_to_ap(const std::string &str) {
// accepts strings formated as 0xffff, ffff, with no importance on case (a or a)
    ap_int<SIZE> ap_val = 0;
    size_t here = str.size();
    size_t low = 0;
    size_t ind = 0;

    if (!here)          // empty
        return ap_uint<SIZE>(0);

    if (str[0] == '0' && (0x20|str[1]) == 'x') // takes care of 0x or 0X
        low = 2;

    while (--here > low) {
        ap_val(3+ind, ind) = nib_to_dec(str[here]);
        ind += 4;
    }
    ap_val(SIZE-1, ind) = nib_to_dec(str[here]);
    return ap_val;
}

#endif

