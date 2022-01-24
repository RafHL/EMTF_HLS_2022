// Assume inside spbits.sv unless said otherwise
// segments per chamber
#ifndef _SPBITS_H_
#define _SPBITS_H_

#define seg_ch  2   // spbits.sv::2

// bit widths of ph and th outputs, reduced precision
// have to be derived from pattern width on top level
#define bw_ph   8   // spbits.sv::6
#define bw_th   7   // spbits.sv::7

// bit widths of ph and th, full precision
#define bw_fph  13  // spbits.sv::10
#define bw_fth  8   // spbits.sv::11

// wiregroup input bit width (0..111)
#define bw_wg   7   // spbits.sv::14

// bit width of dblstrip input (max 80 for ME234/1 with double-width strips)
#define bw_ds   7   // spbits.sv::17
// width of halfstrip input
#define bw_hs   8   // spbits.sv::19

// pattern half-width for stations 3,4
#define pat_w_st3   3 //4;  // spbits.sv::22
    // pattern half-width for station 1
    #define pat_w_st1   pat_w_st3 + 1   // spbits.sv::24
    // number of input bits for stations 3,4
    #define full_pat_w_st3 ((1 << (pat_w_st3+1)) - 1)   // spbits.sv::26
    // number of input bits for st 1
    #define full_pat_w_st1 ((1 << (pat_w_st1+1)) - 1)   // spbits.sv::28
    // width of zero padding for station copies
    #define padding_w_st1 (full_pat_w_st1/2)    // spbits.sv::30
    #define padding_w_st3 (full_pat_w_st3/2)    // spbits.sv::31
    // full pattern widths (aka reduced pattern)
    #define red_pat_w_st3   pat_w_st3 * 2 + 1   // spbits.sv::33
    #define red_pat_w_st1   pat_w_st1 * 2 + 1   // spbits.sv::34

    // number of folds for pattern detectors, do not set to 1
    #define fold    4   // spbits.sv::37
// number of th outputs for ME1/1
#define th_ch11 seg_ch*seg_ch // spbits.sv::39
#define bw_q    4   // spbits.sv::40
#define bw_addr 7   // spbits.sv::41

// strips per section, calculated so ph pattern would cover +/- 8 deg in st 1
#define ph_raw_w    160  // this is determined by chamber coverage in zones.sv  // spbits.sv::44
// should also be divisible by 2

// max possible drifttime
#define max_drift   3   // spbits.sv::48

// bit widths of precise phi and eta outputs
#define bw_phi  12  // spbits.sv::51
#define bw_eta  7   // spbits.sv::52
// width of ph raw hits, max coverage +8 to cover possible chamber displacement
#define ph_hit_w    40+4 //80 + 8   // spbits.sv::54

    // for 20 deg chambers
    #define ph_hit_w20  ph_hit_w    // spbits.sv::57
    // for 10 deg chambers
#define ph_hit_w10  20+4 //40 + 8   // spbits.sv::59

// width of th raw hits, max coverage +8 to cover possible chamber displacement
#define th_hit_w    56 + 8  // spbits.sv::62

    // XXX: endcap, station and cscid are missing from here
    #define n_strips    (station <= 1 && (cscid <= 2 || cscid == 12))                  ? 64 :\
                        (station <= 1 && ((cscid >= 6 && cscid <= 8) || cscid == 14) ) ? 64 :\
                        80; // ME1/1 // ME1/3 // all others respectively    // spbits.sv::68-70

//  Commented out in original file
//  #define n_wg    (station <= 1 && cscid <= 3) ? 48  : // ME1/1
//                  (station <= 1 && cscid >= 6) ? 32  : // ME1/3
//                  (station == 2 && cscid <= 3) ? 112 : // ME2/1
//                  (station >= 3 && cscid <= 3) ? 96  : // ME34/1 
//                  64; // all others

//  // theta range (take +1 because th_coverage contains max th value starting from 0)
//  #define th_coverage
//                       (station <= 1 && cscid <= 2) ? 45  : // ME1/1
//                       (station <= 1 && cscid >= 6) ? 27  : // ME1/3
//                       (station <= 1 && cscid >= 3) ? 39  : // ME2/3
//                       (station == 2 && cscid <= 2) ? 43  : // ME2/1
//                       (station == 2 && cscid >= 3) ? 56  : // ME2/2
//                       (station == 3 && cscid <= 2) ? 34  : // ME3/1
//                       (station == 3 && cscid >= 3) ? 52  : // ME3/2
//                       (station == 4 && cscid <= 2) ? 28  : // ME4/1
//                       (station == 4 && cscid >= 3) ? 50  : // ME4/2 
//                       0;

    // XXX: why commented out? may be in a different file
    // phi range
//  #define ph_coverage (station <= 1 && ((cscid >= 6 && cscid <= 8) || cscid == 14)) ? 15 : // ME1/3 
//                      (station >= 2 && (cscid <= 2 || cscid == 9)) ? 40 : // ME234/1
//                      20; // all others

    // number of th outputs takes ME1/1 th duplication into account
    // Needs to not end with semicolon
    #define th_ch   (station <= 1 && (cscid <= 2 || cscid == 12)) ? (seg_ch*seg_ch) : seg_ch // ME1/1 // all others

    // Commented out in original file
    // In prim_conv.cpp
    // is this chamber mounted in reverse direction?
//  #define ph_reverse   (endcap == 1 && station >= 3) ? 1 :
//                         (endcap == 2 && station <= 3) ? 1 : 0

    #define th_mem_sz       (1 << bw_addr)
    #define th_corr_mem_sz  (1 << bw_addr)

    // multiplier bit width (phi + factor)
    #define mult_bw bw_fph + 11

    // ph zone boundaries for chambers that cover more than one zone
    // hardcoded boundaries must match boundaries in ph_th_match module
    // ***Save to a constant variable, do not use directly [unfortunately]
    // ME1/1
    // ME2/1
    // ME2/2
    // ME3/2
    // ME4/2
    #define ph_zone_bnd1 \
        (station <= 1 && (cscid <= 2 || cscid == 12)) ? 41 : \
        (station == 2 && (cscid <= 2 || cscid ==  9)) ? 41 : \
        (station == 2 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 87 : \
        (station == 3 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 49 : \
        (station == 4 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 49 : \
        127;

    // ME3/2
    #define ph_zone_bnd2 \
        (station == 3 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 87 : \
        127;

    #define rpc_ph_zone_bnd1 \
        (station <= 1 && (cscid <= 2 || cscid == 12)) ? 41 : \
        (station == 2 && (cscid <= 2 || cscid ==  9)) ? 41 : \
        (station == 2 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 87 : \
        (station == 3 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 49 : \
        (station == 4 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 49 : \
        127;

    // ME3/2
    #define rpc_ph_zone_bnd2 \
        (station == 3 && ((cscid >= 3 && cscid <= 8) || cscid == 10)) ? 87 : \
        127;

    #define zone_overlap    2 // spbits.sv::125

// sorter parameters
    #define bwr     6   // rank width
    #define bpow    7   // (1 << bpow) is count of input ranks
    #define cnr     (1 << bpow) // internal rank count
    #define cnrex   ph_raw_w    // actual input rank count, must be even

#endif
