// include this after ap_int.h

enum A2H_opts {
    // Flags for ap_to_hex, to use '|' with each other
    wLeadingZeros = 1,
    w0x = 2
};

char dec_to_nib(char a) {
    if (a >= 10) return a - 10 + 'a';
    else         return a + '0';
}

template <unsigned int SIZE>
std::string apu_to_hex(const ap_uint<SIZE> &ap_val, char opt = 0) {
// opt[0] is wLeadingZeros, for 0000000030
// opt[1] is w0x,           for 0x30
    std::string str = "";           // Empty string
    long long width = SIZE - 1;
    long long max = width - 4;
    long long ind = 0;

    if (max > 0) {
        while (ind < max) {
            str = dec_to_nib(ap_val(3+ind, ind)) + str;
            ind += 4;
        }
    }
    str = dec_to_nib(ap_val(width, ind)) + str;
    if (!(opt & 1)) {               // Remove leading zeros
        ind = 0;
        while (str[ind++] == '0');
        if (!str[--ind]) // If at null pointer, add digit
            ind--;
        if (ind > 0)
            str = str.substr(ind);
    }
    if (opt & 2)                    // Add "0x"
        str = "0x" + str;
    return str;
}

template <unsigned int SIZE>
std::string ap_to_hex(const ap_int<SIZE> &ap_val, char opt = 0) {
// opt[0] is wLeadingZeros, for 0000000030
// opt[1] is w0x,           for 0x30
    std::string str = "";           // Empty string
    long long width = SIZE - 1;
    long long max = width - 4;
    long long ind = 0;

    if (max > 0) {
        while (ind < max) {
            str = dec_to_nib(ap_val(3+ind, ind)) + str;
            ind += 4;
        }
    }
    str = dec_to_nib(ap_val(width, ind)) + str;
    if (!(opt & 1)) {               // Remove leading zeros
        ind = 0;
        while (str[ind++] == '0');
        if (!str[--ind]) // If at null pointer, add digit
            ind--;
        if (ind > 0)
            str = str.substr(ind);
    }
    if (opt & 2)                    // Add "0x"
        str = "0x" + str;
    return str;
}

