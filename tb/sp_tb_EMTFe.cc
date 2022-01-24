///////////////////////////////////////////////////////////////////////////////
// Company: UF
// Engineer: Madorsky, translated into C++ by Rafael Hernandez
//
// Create Date:   10:40:22 03/20/2010
// Design Name:   prim_conv_sector
// Module Name:   C:/Users/madorsky/projects/ise/tf_slhc/sp_tf.v
// Project Name:  tf_slhc
// Target Device:  V6
// Tool versions:  11.4
// Description: test fixture for entire sp design
//
// Dependencies: all sp files
//
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
//
////////////////////////////////////////////////////////////////////////////////
#include <iostream> // printing
#include <fstream>  // Files
#include <cstdio>   // sscanf
#include <vector>
#include <string>   // String manipulation
#include <stdexcept>
#ifdef _LOCAL_AP_INT_H_
  #include "../include/ap_int.h"
#else
  #include <ap_int.h>
#endif
#include "hex_to_ap.h"
#include "../interface/spbits.h"
#include "../interface/sp_EMTFe.h"

//#define max_ev 20
//#define max_ev 200
#define max_ev 4000
//#define max_ev 8000
//#define max_ev 9425
#define rpc_delay 0 // delay of rpc data relative to csc
//#define GOLD

// GOLD is to check against or make a golden reference for CoSim
// _LOCAL_AP_INT_H_ is for g++ jobs outside of Xilinx environment or situations
//     where one would rather use the 2018 ap_int library packaged alongside
//     this HLS firmware in /include

// NOTE: modify parameters below to match endcap/sector and file path
#define endcap_p 1
#define sector_p 1

#define BIT(a) (1 << a)

template <unsigned int SIZE>
void file2arrayu(const std::string &filename, ap_uint<SIZE> *array, size_t size) {
    size_t i = 0;
    std::ifstream ifile(filename);
    std::string   str;
    if (!ifile.is_open())
        throw std::runtime_error("Connot open lut file " + filename);
    // Get data
    while (i < size && !ifile.eof()) {
        ifile >> str;
        array[i++] = hex_to_apu<SIZE>(str);
    }
    ifile.close();
}

template <unsigned int SIZE>
void file2array(const std::string &filename, ap_int<SIZE> *array, size_t size) {
    size_t i = 0;
    std::ifstream ifile(filename);
    std::string   str;
    if (!ifile.is_open())
        throw std::runtime_error("Connot open lut file " + filename);
    // Get data
    while (i < size && !ifile.eof()) {
        ifile >> str;
        array[i++] = hex_to_apu<SIZE>(str);
    }
    ifile.close();
}

int main(int argc, char **argv) {
    using namespace std;

    string dpath;
    string gold_outn;
    string ifilename;

    // Only necessary changes are to the following variables, if at all:
    if (argc > 1) {
        dpath = string(argv[1]);
        cout << "Using dpath: '" << dpath << "'" << std::endl << std::endl;
    } else {
        dpath = "emtf_data/";
    }
    //ifilename = dpath + "/NikhilInputs.txt";
    ifilename = dpath+"/raw_input_100_2GeV_12to24eta_4T_allE1S1_edit.txt";
    //ifilename = dpath+"/raw_input_100_2GeV_12to24eta_4T_allE1S1.txt";
    //ifilename = dpath+"/raw_input_100_2GeV_12to24eta_4T.txt";
    //ifilename = dpath+"/RInputs.txt";
    //ifilename = dpath+"/data.dat";
    //ifilename = "Event609207399.txt";
    //ifilename = "/home/hvuser/mrcarver/ConvTest/St1R1_2_fix.txt";
    gold_outn = dpath + "/sp_tf_gold.out";

    #include "../interface/zns_bnd.h"
    // io
    ap_uint<2>  vf[12][9];        // valid flag // TODO: 8_21 14 to 12
    ap_uint<4>  ql[12][9][seg_ch]; // quality // TODO: 8_21 6 to 12
    ap_uint<7>  wg[6][9][seg_ch]; // wiregroup
    ap_uint<8>  hs[6][9][seg_ch]; // halfstrip
    ap_uint<4>  cp[6][9][seg_ch]; // CLCT pattern

    ap_uint<11> rph[6][9][seg_ch]; // TODO: 8_21 full dimm change
    ap_uint<5>  rth[6][9][seg_ch];
    ap_uint<6>  time[12][9][seg_ch];
    ap_uint<10> bnd[12][9][seg_ch]; // TODO: fill with something.

    ap_int<14>  model_out[emtf::phase2::model_config::n_out];
    ap_uint<13> r_out;

    ap_uint<bw_fph>  uut_ph   [12][9][seg_ch];
    ap_uint<bw_th>   uut_th11 [3][3][th_ch11];
    ap_uint<bw_th>   uut_th   [12][9][seg_ch];
    ap_uint<seg_ch>  uut_vl   [12][9];
    ap_uint<7>       uut_phzvl[12][9];
    ap_uint<seg_ch>  uut_me11a[3][3];
    ap_uint<4>       uut_cpatr[6][9][seg_ch];

    ap_uint<13> r_in;
    ap_uint<1>  we;
    ap_uint<2>  sel;
    ap_uint<9>  cs[6];
    ap_uint<bw_addr> addr;

    // event storage
    // 2 from bx_jitter
    //ap_uint<seg_ch>   valid    [max_ev][14][9];
    // Had stack space issue, rather than change ulimit we chose using heap/free-store via vectors
    vector<vector<vector<ap_uint<seg_ch>>>> valid (
        max_ev + rpc_delay + 2, vector<vector<ap_uint<seg_ch>>>(
            12, vector<ap_uint<seg_ch>>(
                9, ap_uint<seg_ch>(0)
            )
        )
    );
    //ap_uint<4>        quality  [max_ev][6][9][seg_ch];
    vector<vector<vector<vector<ap_uint<4>>>>> quality (
        max_ev + 2, vector<vector<vector<ap_uint<4>>>>( // extra 2 to account for bx_jitter
            6, vector<vector<ap_uint<4>>>(
                9, vector<ap_uint<4>>(
                    seg_ch, ap_uint<4>(0)
                )
            )
        )
    );
    //ap_uint<7>        wiregroup[max_ev][6][9][seg_ch];
    vector<vector<vector<vector<ap_uint<7>>>>> wiregroup (
        max_ev + 2, vector<vector<vector<ap_uint<7>>>>(
            6, vector<vector<ap_uint<7>>>(
                9, vector<ap_uint<7>>(
                    seg_ch, ap_uint<7>(0)
                )
            )
        )
    );
    //ap_uint<bw_hs>    hstrip   [max_ev][6][9][seg_ch];
    vector<vector<vector<vector<ap_uint<bw_hs>>>>> hstrip (
        max_ev + 2, vector<vector<vector<ap_uint<bw_hs>>>>(
            6, vector<vector<ap_uint<bw_hs>>>(
                9, vector<ap_uint<bw_hs>>(
                    seg_ch, ap_uint<bw_hs>(0)
                )
            )
        )
    );
    //ap_uint<4>        clctpat  [max_ev][6][9][seg_ch];
    vector<vector<vector<vector<ap_uint<4>>>>> clctpat (
        max_ev + 2, vector<vector<vector<ap_uint<4>>>>(
            6, vector<vector<ap_uint<4>>>(
                9, vector<ap_uint<4>>(
                    seg_ch, ap_uint<4>(0)
                )
            )
        )
    );

    // 2 from bx_jitter
    // Changed by Rafael to [][8] from [][6] because 2 Phase-2 iRPC chambers
    //ap_uint<11>       rpc_ph [max_ev+rpc_delay][7][8][seg_ch];
    vector<vector<vector<vector<ap_uint<11>>>>> rpc_ph (
        max_ev + rpc_delay + 2, vector<vector<vector<ap_uint<11>>>>(
            6, vector<vector<ap_uint<11>>>(
                9, vector<ap_uint<11>>(
                    seg_ch, ap_uint<11>(0x7ff)
                )
            )
        )
    );
    // 2 from bx_jitter
    //ap_uint<5>        rpc_th [max_ev+rpc_delay][7][8][seg_ch];
    vector<vector<vector<vector<ap_uint<5>>>>> rpc_th (
        max_ev + rpc_delay + 2, vector<vector<vector<ap_uint<5>>>>(
            6, vector<vector<ap_uint<5>>>(
                9, vector<ap_uint<5>>(
                    seg_ch, ap_uint<5>(0x1f)
                )
            )
        )
    );

    // 2 from bx_jitter
    //ap_uint<5>        bend [max_ev+rpc_delay][7][8][seg_ch];
    vector<vector<vector<vector<ap_uint<10>>>>> bend (
        max_ev + rpc_delay + 2, vector<vector<vector<ap_uint<10>>>>(
            12, vector<vector<ap_uint<10>>>(
                9, vector<ap_uint<10>>(
                    seg_ch, ap_uint<10>(0)
                )
            )
        )
    );

    unsigned int      i = 0, j = 0, sn;
#if defined(GOLD) || defined(GOLD_FULL)
    unsigned int      has_gold;
#endif
    unsigned int      v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11;
    ap_uint<3>        pr_cnt[7][8];
    ap_uint<3>        pr_cnt_rpc[7][8];
    unsigned int      _event;
    ap_uint<10>       _bx_jitter;
    ap_uint<2>        _endcap;
    ap_uint<3>        _sector;
    ap_uint<2>        _subsector;
    ap_uint<3>        _station, old_station;
    ap_uint<4>        _cscid;
    ap_uint<4>        _bend;
    ap_uint<11>       _halfstrip;
    ap_uint<2>        _valid;
    ap_uint<4>        _quality;
    ap_uint<4>        _pattern;
    ap_uint<7>        _wiregroup;
    unsigned int      ist, icid, ipr, iev/*, iz, ir*/;
    string            line;
    ifstream          in;
#if defined(GOLD) || defined(GOLD_FULL)
    ifstream          gold_out;
    ofstream          gold_file;
#endif
    ofstream          best_tracks, sim_out, best_tracks_short;
    ap_uint<32>       ev = 0;
    ap_uint<7>        good_ev = 0; // Was 5, but need 7 for st_cnt for-loop
    unsigned int      good_ev_cnt = 0;
    unsigned int      st_cnt; // station count
    unsigned int      /*zi, is,*/ si, ip;

    //time              begin_time, end_time, elapsed_time;
    //ap_uint<12>       phi, a, b, d;
    //ap_uint<1>        pts;

    // precise phi and theta of best tracks
    // [best_track_num]
    ap_uint<bw_fph>   bt_phi[3];
    ap_uint<bw_th>    bt_theta[3];
    ap_uint<4>        bt_cpattern[3][4];
    // ph and th deltas from best stations
    // [best_track_num], last index: [0] - best pair of stations, [1] - second best pair
    ap_uint<bw_fph>   bt_delta_ph[3][6];
    ap_uint<bw_th>    bt_delta_th[3][6];
    ap_uint<6>        bt_sign_ph[3];
    ap_uint<6>        bt_sign_th[3];
    // ranks [best_track_num]
    ap_uint<bwr+1>    bt_rank[3];
    // segment IDs
    // [best_track_num][station 0-3]
    ap_uint<seg_ch>   bt_vi[3][5]; // valid
    ap_uint<2>        bt_hi[3][5]; // bx index
    ap_uint<4>        bt_ci[3][5]; // chamber
    ap_uint<5>        bt_si[3]; // segment
    ap_uint<1>        lat_test = 0; // latency test mode

    ap_uint<8>        gmt_phi[3]; // phi for gmt
    ap_uint<9>        gmt_eta[3]; // eta for gmt

    // NOTE: need ptlut.dat file, currently empty
    // Can just compare address values later, so not essential
//#define PTLUT_SIZE 536870912
//    vector<unsigned int> ptlut(PTLUT_SIZE, 0); // 1/2G x 32 bits, each word carries two 9-bit values
    //unsigned int      pt_word;
    //unsigned int      pt_word_rev;
    ap_uint<9>        pt_value;
#define brev(a) (a(7,0), a(15,8), a(23,16), a(31,24))

    ap_uint<4>        gmt_qlt[3]; // quality for gmt
    ap_uint<3>        gmt_crg;
    ap_uint<30>       ptlut_addr[3]; // ptlut addresses for best 3 muons
    ap_uint<32>       ptlut_cs[3]; // pre-decoded chip selects
    ap_uint<3>        ptlut_addr_val; // ptlut address valid flags
    ap_uint<64>       cppf_rxd[7][3]; // cppf rx data, 3 frames x 64 bit, for 7 links
    ap_uint<7>        cppf_rx_valid = 0x7f; // cprx data valid flags, always valid

    // IMPORTANT: overriden for defines above
    //// NOTE: modify 5 parameters below to match endcap/sector and file path
    //ap_uint<1>        endcap = 1; // 0=ME+ 1=ME-
    //ap_uint<3>        sector = 5; // sector #-1
    ap_uint<1>        endcap = endcap_p - 1;
    ap_uint<3>        sector = sector_p - 1;
    string            fes_str("_endcap_" + to_string(endcap_p) + "_sec_" + to_string(sector_p) + "_");
    string            fest_str("_endcap_" + to_string(endcap_p) + "_sect_" + to_string(sector_p));
//#define fes  "_endcap_2_sec_6_" // endcap and sector numbers for LUT file names
//#define fest "_endcap_2_sect_6" // endcap and sector numbers for LUT file names
#define fes fes_str
#define fest fest_str

    /*unsigned int      ibx, ich, isg, kp;*/
    // Instantiate the Unit Under Test (UUT)
    //sp uut;

    /*unsigned int ph_high_prec, ph_low_prec, ph_zone_phi;*/
    unsigned int k;
    //unsigned int /*gz, gn, gs,*/ f, stb;
    unsigned int first_stub, station_cnt;

    //sim_out.open("sp_tf.out");
    sim_out.open(dpath+"/sp_ehls_r.out");
    //sim_out.open("/home/hvuser/mrcarver/ConvTest/S1R1_closer.out");


    sim_out << "endcap: " << endcap << " sector: " << sector << " strings:     " << fes_str << "     " << fest_str << endl;

    // write parameters to primitive converters
    // ME1
    #include "fill_params.h"

    // read PT LUT
    ifstream ptfile(dpath+"/ptlut.dat");
//    for (unsigned int i = 0; i < PTLUT_SIZE; i++)
//        ptfile >> ptlut[i];
    ptfile.close();

    // initialize single event storage
    for (ist = 0; ist < 6; ist++)
        for (icid = 0; icid < 9; icid++)
            for (ipr = 0; ipr < seg_ch; ipr=ipr+1) {
                vf [ist][icid][ipr] = 0;
                ql [ist][icid][ipr] = 0;
                wg [ist][icid][ipr] = 0;
                hs [ist][icid][ipr] = 0;
                cp [ist][icid][ipr] = 0;
                rph[ist][icid][ipr] = 0x7ff;
                rth[ist][icid][ipr] = 0x1f;
                bnd[ist][icid][ipr] = 0;
                time[ist][icid][ipr] = 0;
                vf [6+ist][icid][ipr] = 0;
                ql [6+ist][icid][ipr] = 0;
                bnd[6+ist][icid][ipr] = 0;
                time[6+ist][icid][ipr] = 0;
            }

    // Fill pcs' params, th_mem, th_corr_mem via r_in memory interface
    we = 1;
    // Fill th_corr_mem first
    sel = 2; // th_corr_mem
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 2 && j > 0) // th_corr_mem[2][0] is neighbor, rest unused
                continue;
            // Set up cs to light up only one th_corr_mem group of size:th_corr_mem_sz
            for (int c = 0; c < 6; c++)
                cs[c] = 0;
            if (i == 2) // th_corr_mem[2][0] is neighbor, rest unused
                cs[5][j] = 1;
            else
                cs[i][j] = 1;
            for (int k = 0; k < th_corr_mem_sz; k++) {
                // Address is third dimmension, k
                addr = k;
                r_in = ap_uint<13>(th_corr_mem[i][j][k]);
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
            }
        }
    }
    // Fill th_mem next
    sel = 1; // th_mem
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            // Set up cs to light up only one th_mem group of size:seg_ch
            for (int c = 0; c < 6; c++)
                cs[c] = 0;
            cs[i][j] = 1;
            for (int k = 0; k < th_mem_sz; k++) {
                // Address is third dimmension, k
                addr = k;
                r_in = ap_uint<13>(th_mem[i][j][k]);
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
            }
        }
    }
    // Fill params last
    sel = 0; // params
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            // Set up cs to light up only one params group of size:seg_ch
            for (int c = 0; c < 6; c++)
                cs[c] = 0;
            cs[i][j] = 1;
            for (int k = 0; k < 6; k++) {
                // Address is third dimmension, k
                addr = k;
                r_in = ap_uint<13>(params[i][j][k]);
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
            }
        }
    }
    we = 0;

/*
    // Check th_corr_mem first
    sel = 2; // th_corr_mem
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 2 && j > 0)
                continue;
            // Set up cs to light up only one th_corr_mem group of size:th_corr_mem_sz
            for (int c = 0; c < 6; c++) {
                cs[c] = 0;
                if      (c == i && i == 2)
                    cs[5][j] = 1;
                else if (c == i)
                    cs[c][j] = 1;
            }
            for (int k = 0; k < th_corr_mem_sz; k++) {
                // Address is third dimmension, k
                addr = k;
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
                if (r_out != th_corr_mem[i][j][k]) {
                    cout << "ERROR: mismatched th_corr_mem"
                         << "[" << i << "]"
                         << "[" << j << "]"
                         << "[" << k << "] = " << r_out
                         << " expected " << th_corr_mem[i][j][k] << endl;
                }
            }
        }
    }
    // Check th_mem next
    sel = 1; // th_mem
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            // Set up cs to light up only one th_mem group of size:seg_ch
            for (int c = 0; c < 6; c++) {
                cs[c] = 0;
                if (c == i)
                    cs[c][j] = 1;
            }
            for (int k = 0; k < th_mem_sz; k++) {
                // Address is third dimmension, k
                addr = k;
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
                if (r_out != th_mem[i][j][k]) {
                    cout << "ERROR: mismatched th_mem"
                         << "[" << i << "]"
                         << "[" << j << "]"
                         << "[" << k << "] = " << r_out
                         << " expected " << th_mem[i][j][k] << endl;
                }
            }
        }
    }
    // Check params last
    sel = 0; // params
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 9; j++) {
            // Set up cs to light up only one params group of size:seg_ch
            for (int c = 0; c < 6; c++) {
                cs[c] = 0;
                if (c == i)
                    cs[c][j] = 1;
            }
            for (int k = 0; k < 6; k++) {
                // Address is third dimmension, k
                addr = k;
                sp(
                    vf, ql, wg, hs, cp, rph, rth, time, bnd,
                    model_out, r_out,
                #ifdef GOLD_FULL
                    uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
                #endif
                    endcap, sector, lat_test,
                    (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
                    r_in, we, addr, sel, cs
                );
                if (r_out != params[i][j][k]) {
                    cout << "ERROR: mismatched params"
                         << "[" << i << "]"
                         << "[" << j << "]"
                         << "[" << k << "] = " << r_out
                         << " expected " << params[i][j][k] << endl;
                }
            }
        }
    }
*/

    // read events
    in.open(ifilename);

    if (!in.is_open()) {
        sim_out << "Warning: cannot open input file: '" << ifilename << "'" << endl;
        cout    << "Warning: cannot open input file: '" << ifilename << "'" << endl;
        return -1;
    }

#if defined(GOLD) || defined(GOLD_FULL)
    gold_out.open(gold_outn);
    if (!gold_out.is_open()) {
        sim_out << "Warning: cannot open golden reference file" << endl;
        cout    << "Warning: cannot open golden file" << endl;
        gold_file.open(gold_outn);
        if (!gold_file.is_open()) {
            sim_out << "Warning: cannot create golden reference file" << endl;
            cout    << "Warning: cannot create golden file" << endl;
            return -2;
        }
        sim_out << "Info: made golden file'" << gold_outn << "'" << endl;
        cout    << "Info: made golden file'" << gold_outn << "'" << endl;
        has_gold = 0;
    } else {
        has_gold = 1;
        sim_out << "Found golden reference file" << endl;
        cout    << "Found golden reference file" << endl;
    }
#endif // GOLD

    _event = 0;
    //sim_out << "in: " << hex << in << " _event: " << hex << _event << endl;

    while (!in.eof() && _event < max_ev) {
        // read line
        getline(in, line); // Read until \n
        if (in.eof())
            break;
        // sim_out << line << endl;
        // read values
        v0 = v1 = v2 = v3 = v4 = v5 = v6 = v7 = v8 = v9 = v10 = v11 = 0;
        sn = sscanf(
                 line.c_str(), "%u %u %u %u %u %u %u %u %u %u %u %u",
                 &v0,&v1,&v2,&v3,&v4,&v5,&v6,&v7,&v8,&v9,&v10,&v11
             );
        //sim_out<<"read "<<sn<<" items "<<v0<<" "<<v1<<" "<<v2<<" "<<v3<<" "
               //<<v4<<" "<<v5<<" "<<v6<<" "<<v7<<" "<<v8<<" "<<v9<<" "<<v10<<" "
               //<<v11<<endl;
        switch (sn) {
        case 1:
            // end of event
            // clean primitive counters
            for (i = 0; i < 7; i=i+1)
                for (j = 0; j < 8; j++) {
                    pr_cnt[i][j] = 0;
                    pr_cnt_rpc[i][j] = 0;
                }

            j = 0;
            _event = _event + 1;
            first_stub = 1;
            station_cnt = 0;
            //sim_out << "End of event " << _event << endl;
            break;
        case 12:
            // primitive
            //$fwrite (sim_out, "entering primitive *****************************************\n");
            _bx_jitter = v0;
            _endcap = v1;
            _sector = v2;
            _subsector = v3;
            _station = v4;
            if (_station == 1 && _subsector == 1) _station = 0;
            _valid = v5;
            _quality = v6;
            _pattern = v7;
            _wiregroup = v8;
            _cscid = v9;
            _bend = v10;
            _halfstrip = v11;

            // code that adds jitter, for BXA checks
            if (first_stub == 1) old_station = _station;
            first_stub = 0;

            if (old_station != _station)
                station_cnt = station_cnt+1;

            _bx_jitter = 2;
            //if (station_cnt <= 1) _bx_jitter = 0;

            // increase stub counter only when station changes
            old_station = _station;
            // end of BX jitter code

            //sim_out<<"primitive: subsec: "<< _subsector<<"  stat: "<<_station
            //       <<"  val: "<<_valid<<"  hs: "<<_halfstrip
            //       <<"  wg: "<<_wiregroup<<"  q: "_quality<<endl;

            // copy data to the corresponding input
            if (_endcap == endcap_p && _sector == sector_p) {
                if (_valid == 1) { // csc
                    _cscid = _cscid-1; // CSC IDs start from 1 in the input files
                    if (_station > 5)
                        sim_out<<"bad station: "<<_station<<", event: "<<_event<<endl;
                    else if (_cscid > 8)
                        sim_out<<"bad cscid: "<<_cscid<<", station: "<<_station
                               <<", event: "<<_event<<endl;
                    else if (pr_cnt[_station][_cscid] >= seg_ch)
                        sim_out<<"bad segment index. event: "<<_event<<", index: "
                               <<pr_cnt[_station][_cscid]<<", station: "<<_station
                               <<", cscid: "<<_cscid<<endl;
                    else {
                        valid    [_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]] = _valid;
                        quality  [_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]] = _quality;
                        wiregroup[_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]] = _wiregroup;
                        hstrip   [_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]] = _halfstrip;
                        clctpat  [_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]] = _pattern;
                        //if (valid    [_event+_bx_jitter][_station][_cscid][pr_cnt[_station][_cscid]]) {
                            //std::cout << "csc read Valid-> ev: " << _event+_bx_jitter << " st: " << _station
                                             //<< " ch: " << _cscid
                                             //<< " sg: " << pr_cnt[_station][_cscid]
                                             //<< std::endl;
                        //}
                        // increase primitive counter
                        pr_cnt   [_station][_cscid] = pr_cnt[_station][_cscid] + 1;
                    } // else: !if(pr_cnt[_station][_cscid] >= seg_ch)
                } // if (_valid == 1)
                else if (_valid == 2) { // rpc
                    // For rpc, _station is subsector
                    //          _cscid   is RPC chamber
                    //_station = _station-1; // CSC IDs start from 1 in the input files
                    ap_uint<4> csc_eq_st;
                    ap_uint<4> csc_eq_ch;
                    // From coord_delay.sv rpc_subsect/ch table
                    // TODO: Modify for new RE3,4/1
                    if (_station == 6) { // Neighbors at subsector 0
                        //std::cout << "Neighbor: rpcid " << _cscid << " subsect " << _station << std::endl;
                        csc_eq_st = 5;
                        csc_eq_ch = (_cscid == ap_uint<4>(0)) ? ap_uint<4>(1) :
                                    (_cscid == ap_uint<4>(1)) ? ap_uint<4>(4) :
                                    (_cscid == ap_uint<4>(2)) ? ap_uint<4>(6) :
                                    (_cscid == ap_uint<4>(3)) ? ap_uint<4>(6) : // Andrew B, 5/3/17 chamber 2 means 2 or 3
                                    (_cscid == ap_uint<4>(4)) ? ap_uint<4>(8) :
                                    (_cscid == ap_uint<4>(5)) ? ap_uint<4>(8) : // Andrew B, 5/3/17 chamber 4 means 4 or 5
                                                                ap_uint<4>(-1);
                    } else { // RPC
                        //std::cout << "rpcid " << _cscid << " subsect " << _station << std::endl;
                        csc_eq_st = (_cscid == ap_uint<4>(0) && _station < ap_uint<3>(4)) ? ap_uint<4>(0) : // subsectors 4:6 have [csc_st=1][x]
                                    (_cscid == ap_uint<4>(0)) ? ap_uint<4>(1) :
                                    (_cscid == ap_uint<4>(1)) ? ap_uint<4>(2) :
                                    (_cscid == ap_uint<4>(2) || _cscid == ap_uint<4>(3))  ? ap_uint<4>(3) : // Andrew B, 5/3/17 chamber 2 means 2 or 3
                                    (_cscid == ap_uint<4>(4) || _cscid == ap_uint<4>(5))  ? ap_uint<4>(4) : // Andrew B. 5/3/17 chamber 4 means 4 or 5
                                                                ap_uint<4>(-1);
                        csc_eq_ch = (_cscid == ap_uint<4>(0) && _station < ap_uint<3>(4)) ? ap_uint<4>(_station + 3) :
                                    (_cscid  == ap_uint<4>(0)) ? ap_uint<4>(_station) :
                                    (_station < ap_uint<3>(7)) ? ap_uint<4>(_station + 3) : // If subsect valid
                                                                ap_uint<4>(-1); // Invalid subsect
                    }
                    // Correct for RE3,4/1
                    ap_uint<13> ph_val_rpc = (_halfstrip, ap_uint<2>(0));
                    ap_uint<9> th_val_rpc = (_wiregroup, ap_uint<2>(0));
                    //ap_uint<9> th_ring1_bnd_approx = 25;
                    //ap_uint<9> th_ring1_bnd_approx = 40;
                    ap_uint<9> th_ring1_bnd_approx = (zns_bnd[2][4][1][0] + zns_bnd[2][4][0][1])/2;
                    std::cout << "Event: " << _event << " th: " << th_val_rpc << " th limit: " << th_ring1_bnd_approx << std::endl;
                    if (csc_eq_st > ap_uint<4>(2) && csc_eq_ch > ap_uint<4>(2) && th_val_rpc <= th_ring1_bnd_approx) { // ring 1
                        if (csc_eq_st == 5) {
                            std::cout << "Fixed " << _event << "'s rpc st: " << csc_eq_st << " ch: " << csc_eq_ch << ", th: " << th_val_rpc << " " << _wiregroup;
                            csc_eq_ch = (csc_eq_ch == ap_uint<4>(6)) ? ap_uint<4>(5) :
                                        (csc_eq_ch == ap_uint<4>(8)) ? ap_uint<4>(7) :
                                                                       csc_eq_ch;
                            std::cout << " to ch: " << csc_eq_ch << std::endl;
                        } else {
                            std::cout << "Fixed " << _event << "'s rpc st: " << csc_eq_st << " ch: " << csc_eq_ch << ", th: " << th_val_rpc << " " << _wiregroup;
                            csc_eq_ch = (csc_eq_ch > 6) ? 2 :
                                        (csc_eq_ch > 4) ? 1 :
                                                          0;
                            std::cout << " to ch: " << csc_eq_ch << std::endl;
                        }
                    }
                    if (pr_cnt_rpc[_station][_cscid] >= seg_ch)
                        sim_out<<"bad RPC segment index. event: "<<_event
                               <<", index: "<<pr_cnt_rpc[_station][_cscid]
                               <<", subsector: "<<_station<<", chamber: "<<_cscid<<endl;
                    else {
                        sim_out<<"rpc hit: st: "<<csc_eq_st
                               <<" ch: "<<csc_eq_ch<<" phi: "<<_halfstrip
                               <<" th: "<<_wiregroup<<endl;
                        rpc_ph [_event + _bx_jitter + rpc_delay][csc_eq_st][csc_eq_ch][pr_cnt_rpc[_station][_cscid]] = _halfstrip; // actually carries phi
                        rpc_th [_event + _bx_jitter + rpc_delay][csc_eq_st][csc_eq_ch][pr_cnt_rpc[_station][_cscid]] = _wiregroup; // actually carries theta
                        valid  [_event + _bx_jitter + rpc_delay][6+csc_eq_st][csc_eq_ch][pr_cnt_rpc[_station][_cscid]] = 1; // actually carries theta
                        bend   [_event + _bx_jitter + rpc_delay][6+csc_eq_st][csc_eq_ch][pr_cnt_rpc[_station][_cscid]] = _bend;
//                        if ( valid  [_event + _bx_jitter + rpc_delay][6+csc_eq_st][csc_eq_ch][pr_cnt_rpc[_station][_cscid]]) {
//                            std::cout << "rpc read Valid-> ev: " << _event + _bx_jitter + rpc_delay << " st: " << 6+csc_eq_st
//                                             << " ch: " << csc_eq_ch
//                                             << " sg: " << pr_cnt_rpc[_station][_cscid]
//                                             << std::endl;
//                        }
                        // increase primitive counter
                        pr_cnt_rpc[_station][_cscid] = pr_cnt_rpc[_station][_cscid] + 1;
                    } // else: !if(pr_cnt_rpc[_station][_cscid] >= seg_ch)
                } // if (_valid == 2)
            } // if (_endcap == endcap_p || _sector == sector_p)
            break;
        default:
            sim_out<<"Collected a wrong number of samples."
                   <<" event: "<<_event<<", station: "<<_station
                   <<", cscid: "<<_cscid<<endl;
            cout   <<"Collected a wrong number of samples."
                   <<" event: "<<_event<<", station: "<<_station
                   <<", cscid: "<<_cscid<<endl;
            return -1;
        }
    }

    //elapsed_time = 0;

    best_tracks.open(dpath+"/best_tracks_2nd.out");
    best_tracks_short.open(dpath+"/best_tracks_short_2nd.out");

    // read params registers
    sim_out <<  "st: 0  cid: 0  "; for (si=0; si<6; si++) sim_out << setw(5) << params[0][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 1  "; for (si=0; si<6; si++) sim_out << setw(5) << params[0][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 2  "; for (si=0; si<6; si++) sim_out << setw(5) << params[0][2][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 0  "; for (si=0; si<6; si++) sim_out << setw(5) << params[1][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 1  "; for (si=0; si<6; si++) sim_out << setw(5) << params[1][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 2  "; for (si=0; si<6; si++) sim_out << setw(5) << params[1][2][si] << " "; sim_out << endl;

    sim_out <<  "st: 0  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 0  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[0][8][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 1  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[1][8][si] << " "; sim_out << endl;

    sim_out <<  "st: 2  cid: 0  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 1  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 2  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][2][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 2  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[2][8][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 0  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 1  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 2  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][2][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 3  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[3][8][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 0  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 1  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 2  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][2][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][8][si] << " "; sim_out << endl;
    sim_out <<  "st: 4  cid: 0  "; for (si=0; si<4; si++) sim_out << setw(5) << params[4][0][si] << " "; sim_out << endl;

    sim_out <<  "neighbor sector:" << endl;
    sim_out <<  "st: 5  cid: 0  "; for (si=0; si<6; si++) sim_out << setw(5) << params[5][0][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 1  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][1][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 2  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][2][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 3  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][3][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 4  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][4][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 5  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][5][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 6  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][6][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 7  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][7][si] << " "; sim_out << endl;
    sim_out <<  "st: 5  cid: 8  "; for (si=0; si<4; si++) sim_out << setw(5) << params[5][8][si] << " "; sim_out << endl;

    for (i = 1; i <= max_ev; i++) {
        for (k = 0; k < 5; k++) {
            for (j = 0; j < 9; j++) {
                for (si = 0; si < seg_ch; si++) {
                    ql[k][j][si] = 0;
                    wg[k][j][si] = 0;
                    hs[k][j][si] = 0;
                    cp[k][j][si] = 0;
                }
            }
        }

        good_ev = 0;
        st_cnt = 0;
        for (ist = 0; ist < 6; ist++) { // station
            for (icid = 0; icid < 9; icid++) { // chamber id
                for (si = 0; si < seg_ch; si++) { // stub
                    vf [ist][icid][si] = valid    [ev][ist][icid][si];
                    ql [ist][icid][si] = quality  [ev][ist][icid][si];
                    wg [ist][icid][si] = wiregroup[ev][ist][icid][si];
                    hs [ist][icid][si] = hstrip   [ev][ist][icid][si];
                    cp [ist][icid][si] = clctpat  [ev][ist][icid][si];
                    bnd[ist][icid][si] = bend     [ev][ist][icid][si];
                    //if (valid[ev][ist][icid][si]) {
                        //std::cout << "memcpy Valid-> ev: " << ev << " st: " << ist
                                         //<< " ch: " << icid
                                         //<< " sg: " << si
                                         //<< std::endl;
                    //}
                    //                         $fwrite(sim_out, "ev: %d st: %d ch: %d prim: %d   q: %h w: %h s: %h\n",
                    //                             ev, ist, icid, ipr, ql[ist][icid], wg[ist][icid], hs[ist][icid]);

                    // check if there is chamber data, update good event station mask
                    if (ql[ist][icid][si] > 0) good_ev[ist] = 1;
                }
            } // for (icid = 0; icid < 9; icid=icid+1)
            // count stations in this event
            if (good_ev[ist]) st_cnt = st_cnt + 1;
        }
        for (ist = 0; ist < 6; ist++) { // subsector
            for (icid = 0; icid < 9; icid++) { // rpc chamber
                for (si = 0; si < seg_ch; si++) { // hit
                    vf[ist+6][icid][si] = valid[ev][ist+6][icid][si];
                    bnd[ist+6][icid][si] = bend[ev][ist+6][icid][si];
                    rth[ist][icid][si] = rpc_th[ev][ist][icid][si];
                    rph[ist][icid][si] = rpc_ph[ev][ist][icid][si];
                    //if (valid[ev][6+ist][icid][si]) {
                        //std::cout << "memcpy rpc Valid-> ev: " << ev << " st: " << ist
                                         //<< " ch: " << icid
                                         //<< " sg: " << si
                                         //<< std::endl;
                    //}
                }
            } // for (icid = 0; icid < 9; icid++)
            // count stations in this event
            if (good_ev[ist]) st_cnt++;
        }

//        Commented by Rafael to add in ^ rth/rph/vf
//        for (ist = 0; ist < 7; ist++) { // subsector
//            for (icid = 0; icid <= 8; icid++) { // rpc chamber
//                for (si = 0; si < seg_ch; si++) { // hit
//                    f = icid/3; // frame Changed by Rafael to 3 to account for RE3/1 and RE4/1
//                    stb = ((icid%2)*2+si); // stub
//                    cppf_rxd[ist][f](stb*16+15, stb*16) = (rpc_th [ev][ist][icid][si], rpc_ph [ev][ist][icid][si]);
//                    //cppf_rxd[ist][f](stb*16+15, stb*16) = 0xffff;
//                }
//            } // for (icid = 0; icid < 9; icid++)
//            // count stations in this event
//            if (good_ev[ist]) st_cnt++;
//        }

        ev++;
        // count event as good if more than 2 stations, other than 3-4
        if (good_ev != 0 && good_ev != 1 && good_ev != 2 && good_ev != 4 && good_ev != 8 &&
            good_ev != 16 && good_ev != 24)
          good_ev_cnt = good_ev_cnt+1;
        //if (good_ev == 24) st_cnt = 7; // 3-4 tracks marked as having 7 stations
        //begin_time = $time;

        iev = i;
        cout << endl;
        sim_out << endl << "event:" << setw(8) << iev << endl;
        cout    <<         "event:" << setw(8) << iev << endl;

        // Top prepares inputs and now we write them
        //for (int ist = 0; ist < 6; ist++) {
            //for (int ich = 0; ich < 9; ich++) {
                //for (int isg = 0; isg < 2; isg++) {
                    //if (vf[ist][ich][isg]) {
                        //std::cout << "Valid-> st: " << ist
                                         //<< " ch: " << ich
                                         //<< " sg: " << isg
                                         //<< std::endl;
                    //}
                    //if (valid[ev][ist][ich][isg]) {
                        //std::cout << "Event Valid-> st: " << ist
                                         //<< " ch: " << ich
                                         //<< " sg: " << isg
                                         //<< std::endl;
                    //}
                //}
            //}
        //}
        sp(
            vf, ql, wg, hs, cp, rph, rth, time, bnd,
            model_out, r_out,
        #ifdef GOLD_FULL
            uut_ph, uut_th11, uut_th, uut_vl, uut_phzvl, uut_me11a, uut_cpatr,
        #endif
            endcap, sector, lat_test,
            (ap_uint<1>(0), ap_uint<1>(0), ap_uint<4>(9), ap_uint<8>(8)), // en_2mu, en_single, delay_2mu, th_window
            r_in, we, addr, sel, cs
        );

        // TODO: make temporary sp with outputs that are pcs outputs. If not,
        // will optimize away since they don't come out
#if defined(GOLD) || defined(GOLD_FULL)
        if (has_gold) {
            unsigned int ev_num;
            if (gold_out.peek() == EOF) {
                cout    << "Error: insufficient data in golden file" << endl;
                sim_out << "Error: insufficient data in golden file" << endl;
                return -3;
            }
            gold_out >> ev_num;
            if (ev_num != iev) {
                cout    << "Warning: event mismatch: " << iev << " != " << ev_num << endl;
                sim_out << "Warning: event mismatch: " << iev << " != " << ev_num << endl;
            }
            //$fwrite(sim_out, "\nevent:%8d\n", iev);

    #ifdef GOLD_FULL
            // ph to golden
            ap_uint<bw_fph> ph_g = 0;
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++)
                    for (int k = 0; k < seg_ch; k++) {
                        //$fwrite(gold_out, "%x ", uut_ph[i][j][k]);
                        if (gold_out.peek() == EOF) {
                            cout    << "Error: insufficient data in golden file" << endl;
                            sim_out << "Error: insufficient data in golden file" << endl;
                            return -3;
                        }
                        gold_out >> ph_g;
                        if (uut_vl[i][j][k] && uut_ph[i][j][k] != ph_g) {
                            cout << "Error: ph["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                 << uut_ph[i][j][k] << " != " << ph_g
                                 << " at event: " << ev_num << endl;
                            sim_out << "Error: ph["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                    << uut_ph[i][j][k] << " != " << ph_g
                                    << " at event: " << ev_num << endl;
                            return -1;
                        }
                    }

            // th11 to golden
            ap_uint<bw_th> th11_g = 0;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++) {
                    if (i == 2 && j > 0) // th11[2][0] is neighbor, rest unused
                        continue;
                    for (int k = 0; k < th_ch11; k++) {
                        //$fwrite(gold_out, "%x ", uut_th11[i][j][k]);
                        if (gold_out.peek() == EOF) {
                            cout    << "Error: insufficient data in golden file" << endl;
                            sim_out << "Error: insufficient data in golden file" << endl;
                            return -3;
                        }
                        gold_out >> th11_g;
                        if (uut_vl[i][j] == 0) // VL==00, none valid
                            continue;
                        if (uut_vl[i][j] == 1 && k != 0) // VL==01, th[0] valid
                            continue;
                        if (uut_vl[i][j] == 2 && k != 3) // VL==10, th[3] valid
                            continue;
                        // VL==11, th[0:3] valid
                        //cout << "th11[" << i << "][" << j << "][" << k << "] vl:" << uut_vl[i][j][0] << " " << uut_vl[i][j][1] << endl;
                        if (uut_th11[i][j][k] != th11_g) {
                            cout << "Error: th11["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                 << uut_th11[i][j][k] << " != " << th11_g
                                 << " at event: " << ev_num << endl;
                            sim_out << "Error: th11["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                    << uut_th11[i][j][k] << " != " << th11_g
                                    << " at event: " << ev_num << endl;
                            return -1;
                        }
                    }
                }

            // th to golden
            ap_uint<bw_th> th_g = 0;
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++) {
                    if (i < 2 && j < 3) // th[0:1][0:2] unused
                        continue;
                    if (i == 5 && j == 0) // PC11 neighbor
                        continue;
                    for (int k = 0; k < seg_ch; k++) {
                        //$fwrite(gold_out, "%x ", uut_th[i][j][k]);
                        if (gold_out.peek() == EOF) {
                            cout    << "Error: insufficient data in golden file" << endl;
                            sim_out << "Error: insufficient data in golden file" << endl;
                            return -3;
                        }
                        gold_out >> th_g;
                        if (uut_vl[i][j][k] && uut_th[i][j][k] != th_g) {
                            cout << "Error: th["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                 << uut_th[i][j][k] << " != " << th_g
                                 << " at event: " << ev_num << endl;
                            sim_out << "Error: th["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                    << uut_th[i][j][k] << " != " << th_g
                                    << " at event: " << ev_num << endl;
                            return -1;
                        }
                    }
                }

            // vl to golden
            ap_uint<bw_th> vl_g = 0;
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++) {
                    //$fwrite(gold_out, "%x ", uut_vl[i][j]);
                    if (gold_out.peek() == EOF) {
                        cout    << "Error: insufficient data in golden file" << endl;
                        sim_out << "Error: insufficient data in golden file" << endl;
                        return -3;
                    }
                    gold_out >> vl_g;
                    if (uut_vl[i][j] != vl_g) {
                        cout << "Error: vl["<<i<<"]["<<j<<"] mismatch: "
                             << uut_vl[i][j] << " != " << vl_g
                             << " at event: " << ev_num << endl;
                        sim_out << "Error: vl["<<i<<"]["<<j<<"] mismatch: "
                                << uut_vl[i][j] << " != " << vl_g
                                << " at event: " << ev_num << endl;
                        return -1;
                    }
                }

            // phzvl to golden
            ap_uint<7> phzvl_g = 0;
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++) {
                    //$fwrite(gold_out, "%x ", uut_phzvl[i][j]);
                    if (gold_out.peek() == EOF) {
                        cout    << "Error: insufficient data in golden file" << endl;
                        sim_out << "Error: insufficient data in golden file" << endl;
                        return -3;
                    }
                    gold_out >> phzvl_g;
                    if (uut_phzvl[i][j] != phzvl_g) {
                        cout << "Error: phzvl["<<i<<"]["<<j<<"] mismatch: "
                             << uut_phzvl[i][j] << " != " << phzvl_g
                             << " at event: " << ev_num << endl;
                        sim_out << "Error: phzvl["<<i<<"]["<<j<<"] mismatch: "
                                << uut_phzvl[i][j] << " != " << phzvl_g
                                << " at event: " << ev_num << endl;
                        return -1;
                    }
                }

            // me11a to golden
            ap_uint<seg_ch> me11a_g = 0;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++) {
                    if (i == 2 && j > 0) // me11a[2][0] is neighbor, rest unused
                        continue;
                    //$fwrite(gold_out, "%x ", uut_me11a[i][j]);
                    if (gold_out.peek() == EOF) {
                        cout    << "Error: insufficient data in golden file" << endl;
                        sim_out << "Error: insufficient data in golden file" << endl;
                        return -3;
                    }
                    gold_out >> me11a_g;
                    if (uut_me11a[i][j] != me11a_g) {
                        cout << "Error: me11a["<<i<<"]["<<j<<"] mismatch: "
                             << uut_me11a[i][j] << " != " << me11a_g
                             << " at event: " << ev_num << endl;
                        sim_out << "Error: me11a["<<i<<"]["<<j<<"] mismatch: "
                                << uut_me11a[i][j] << " != " << me11a_g
                                << " at event: " << ev_num << endl;
                        return -1;
                    }
                }

            // cpatr to golden
            ap_uint<4> cpatr_g = 0;
            for (int i = 0; i < 6; i++)
                for (int j = 0; j < 9; j++)
                    for (int k = 0; k < seg_ch; k++) {
                        //$fwrite(gold_out, "%x ", uut_cpatr[i][j][k]);
                        if (gold_out.peek() == EOF) {
                            cout    << "Error: insufficient data in golden file" << endl;
                            sim_out << "Error: insufficient data in golden file" << endl;
                            return -3;
                        }
                        gold_out >> cpatr_g;
                        if (uut_vl[i][j][k] && uut_cpatr[i][j][k] != cpatr_g) {
                            cout << "Error: cpatr["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                 << uut_cpatr[i][j][k] << " != " << cpatr_g
                                 << " at event: " << ev_num << endl;
                            sim_out << "Error: cpatr["<<i<<"]["<<j<<"]["<<k<<"] mismatch: "
                                    << uut_cpatr[i][j][k] << " != " << cpatr_g
                                    << " at event: " << ev_num << endl;
                            return -1;
                        }
                    }
    #endif // GOLD_FULL

            // model_out to golden
            ap_int<14> model_out_g = 0;
            for (unsigned int i = 0; i < emtf::phase2::model_config::n_out; i++) {
                //$fwrite(gold_out, "%x ", model_out[i]);
                if (gold_out.peek() == EOF) {
                    cout    << "Error: insufficient data in golden file" << endl;
                    sim_out << "Error: insufficient data in golden file" << endl;
                    return -3;
                }
                gold_out >> model_out_g;
                if (model_out[i] != model_out_g) {
                    cout << "Error: model_out["<<i<<"] mismatch: "
                         << model_out[i] << " != " << model_out_g
                         << " at event: " << ev_num << endl;
                    sim_out << "Error: model_out["<<i<<"] mismatch: "
                            << model_out[i] << " != " << model_out_g
                            << " at event: " << ev_num << endl;
                    return -1;
                }
            }

            //// ph_hit to golden
            //ap_uint<ph_hit_w> ph_hit_g;
            //for (int i = 0; i < 14; i++)
                //for (int j = 0; j < 9; j++) {
                    ////$fwrite(gold_out, "%x ", uut_ph_hito[i][j]);
                    //if (gold_out.peek() == EOF) {
                        //cout    << "Error: insufficient data in golden file" << endl;
                        //sim_out << "Error: insufficient data in golden file" << endl;
                        //return -3;
                    //}
                    //gold_out >> ph_hit_g;
                    //if (uut_ph_hito[i][j] != ph_hit_g) {
                        //cout << "Error: ph_hit["<<i<<"]["<<j<<"] mismatch: "
                             //<< uut_ph_hito[i][j] << " != " << ph_hit_g
                             //<< " at event: " << ev_num << endl;
                        //sim_out << "Error: ph_hit["<<i<<"]["<<j<<"] mismatch: "
                                //<< uut_ph_hito[i][j] << " != " << ph_hit_g
                                //<< " at event: " << ev_num << endl;
                        //return -1;
                    //}
                //}
//
            //// ph_zone to golden
            //ap_uint<ph_raw_w> ph_zone_g;
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < 15; j++) {
                    ////$fwrite(gold_out, "%x ", uut_ph_zone[i][j]);
                    //if (gold_out.peek() == EOF) {
                        //cout    << "Error: insufficient data in golden file" << endl;
                        //sim_out << "Error: insufficient data in golden file" << endl;
                        //return -3;
                    //}
                    //gold_out >> ph_zone_g;
                    //if (uut_ph_zone[i][j] != ph_zone_g) {
                        //cout << "Error: ph_zone["<<i<<"]["<<j<<"] mismatch: "
                             //<< uut_ph_zone[i][j] << " != " << ph_zone_g
                             //<< " at event: " << ev_num << endl;
                        //sim_out << "Error: ph_zone["<<i<<"]["<<j<<"] mismatch: "
                                //<< uut_ph_zone[i][j] << " != " << ph_zone_g
                                //<< " at event: " << ev_num << endl;
                        //return -1;
                    //}
                //}
//
            //// ph_ext to golden
            //ap_uint<ph_raw_w> ph_ext_g;
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < 15; j++) {
                    ////$fwrite(gold_out, "%x ", uut_ph_ext[i][j]);
                    //if (gold_out.peek() == EOF) {
                        //cout    << "Error: insufficient data in golden file" << endl;
                        //sim_out << "Error: insufficient data in golden file" << endl;
                        //return -3;
                    //}
                    //gold_out >> ph_ext_g;
                    //if (uut_ph_ext[i][j] != ph_ext_g) {
                        //cout << "Error: ph_ext["<<i<<"]["<<j<<"] mismatch: "
                             //<< uut_ph_ext[i][j] << " != " << ph_ext_g
                             //<< " at event: " << ev_num << endl;
                        //sim_out << "Error: ph_ext["<<i<<"]["<<j<<"] mismatch: "
                                //<< uut_ph_ext[i][j] << " != " << ph_ext_g
                                //<< " at event: " << ev_num << endl;
                        //return -1;
                    //}
                //}
//
            //// ph_rank to golden
            //ap_uint<11> ph_rank_g;
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < ph_rank; j++) {
                    ////$fwrite(gold_out, "%x ", uut_ph_rank[i][j]);
                    //if (gold_out.peek() == EOF) {
                        //cout    << "Error: insufficient data in golden file" << endl;
                        //sim_out << "Error: insufficient data in golden file" << endl;
                        //return -3;
                    //}
                    //gold_out >> ph_rank_g;
                    //if (uut_ph_rank[i][j] != ph_rank_g) {
                        //cout << "Error: ph_rank["<<i<<"]["<<j<<"] mismatch: "
                             //<< uut_ph_rank[i][j] << " != " << ph_rank_g
                             //<< " at event: " << ev_num << endl;
                        //sim_out << "Error: ph_rank["<<i<<"]["<<j<<"] mismatch: "
                                //<< uut_ph_rank[i][j] << " != " << ph_rank_g
                                //<< " at event: " << ev_num << endl;
                        //return -1;
                    //}
                //}

            //$fwrite(gold_out, "\n");
        } else { // If gold doesn't exit, make it!
            gold_file << iev << endl;
            //$fwrite(sim_out, "\nevent:%8d\n", iev);

    #ifdef GOLD_FULL
            // ph to golden
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++)
                    for (int k = 0; k < seg_ch; k++)
                        //$fwrite(gold_out, "%x ", uut_ph[i][j][k]);
                        gold_file << uut_ph[i][j][k] << " ";
            gold_file << endl;

            // th11 to golden
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++) {
                    if (i == 2 && j > 0) // th11[2][0] is neighbor, rest unused
                        continue;
                    for (int k = 0; k < th_ch11; k++)
                        //$fwrite(gold_out, "%x ", uut_th11[i][j][k]);
                        gold_file << uut_th11[i][j][k] << " ";
                }
            gold_file << endl;

            // th to golden
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++) {
                    if (i < 2 && j < 3) // th[0:1][0:2] unused
                        continue;
                    if (i == 5 && j == 0) // PC11 neighbor
                        continue;
                    for (int k = 0; k < seg_ch; k++)
                        //$fwrite(gold_out, "%x ", uut_th[i][j][k]);
                        gold_file << uut_th[i][j][k] << " ";
                }
            gold_file << endl;

            // vl to golden
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++)
                    //$fwrite(gold_out, "%x ", uut_vl[i][j]);
                    gold_file << uut_vl[i][j] << " ";
            gold_file << endl;

            // phzvl to golden
            for (int i = 0; i < 12; i++)
                for (int j = 0; j < 9; j++)
                    //$fwrite(gold_out, "%x ", uut_phzvl[i][j]);
                    gold_file << uut_phzvl[i][j] << " ";
            gold_file << endl;

            // me11a to golden
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++) {
                    if (i == 2 && j > 0) // me11a[2][0] is neighbor, rest unused
                        continue;
                    //$fwrite(gold_out, "%x ", uut_me11a[i][j]);
                    gold_file << uut_me11a[i][j] << " ";
                }
            gold_file << endl;

            // cpatr to golden
            for (int i = 0; i < 6; i++)
                for (int j = 0; j < 9; j++)
                    for (int k = 0; k < seg_ch; k++)
                        //$fwrite(gold_out, "%x ", uut_cpatr[i][j][k]);
                        gold_file << uut_cpatr[i][j][k] << " ";
            gold_file << endl;
    #endif // GOLD_FULL

            // model_out to golden
            for (unsigned int i = 0; i < emtf::phase2::model_config::n_out; i++)
                //$fwrite(gold_out, "%x ", model_out[i]);
                gold_file << model_out[i] << " ";
            gold_file << endl;

            //// ph_hit to golden
            //for (int i = 0; i < 14; i++)
                //for (int j = 0; j < 9; j++)
                    ////$fwrite(gold_out, "%x ", uut_ph_hito[i][j]);
                    //gold_file << uut_ph_hito[i][j] << " ";
            //gold_file << endl;
//
            //// ph_zone to golden
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < 15; j++)
                    ////$fwrite(gold_out, "%x ", uut_ph_zone[i][j]);
                    //gold_file << uut_ph_zone[i][j] << " ";
            //gold_file << endl;
//
            //// ph_ext to golden
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < 15; j++)
                    ////$fwrite(gold_out, "%x ", uut_ph_ext[i][j]);
                    //gold_file << uut_ph_ext[i][j] << " ";
            //gold_file << endl;
//
            //// ph_rank to golden
            //for (int i = 0; i < 7; i++)
                //for (int j = 0; j < ph_raw_w; j++)
                    ////$fwrite(gold_out, "%x ", uut_ph_rank[i][j]);
                    //gold_file << uut_ph_rank[i][j] << " ";
            //gold_file << endl;

            //$fwrite(gold_out, "\n");
        }
#endif // GOLD

        //$fwrite(sim_out, "ph: %d %d %d %d %d\n",
        //      uut_ph[0][3][0], uut_ph[1][3][0], uut_ph[2][3][0], uut_ph[3][3][0], uut_ph[4][3][0]);
        /*
        for (si = 0; si < 7; si = si+1) // subsector
        {
            for (j = 0; j < 3; j = j+1) // frame
              {
                 if (cppf_rxd[si][j] != 64'hffffffffffffffff)
                   $fwrite (sim_out, "CPPF: lnk: %d fr: %d d: %x\n", si, j, cppf_rxd[si][j]);
            }
        }
        */

        // TODO: consider uut_XXXX
        /*
        for (si = 0; si < 4; si++)
            for (ip = 0; ip < 6; ip++)
                for (j = 0; j < 9; j++)
                    for (k = 0; k < 2; k++) {
                        //if (uut_cpatd[si][ip][j][k] == 4'h0 && uut_vld[si][ip][j][k] == 1'h1) { // RPC stub
                              //$fwrite (sim_out, "RPC: hi: %d st: %d ch: %d seg: %d ph: %d th: %d\n",
                                       //si, ip, j, k, uut_phd[si][ip][j][k], uut_thd[si][ip][j][k]);
                        //}
                        //if (uut_cpatd[si][ip][j][k] != 4'h0 && uut_vld[si][ip][j][k] == 1'h1) { // ME stub
                              //$fwrite (sim_out, "ME : hi: %d st: %d ch: %d seg: %d ph: %d th: %d\n",
                                       //si, ip, j, k, uut_phd[si][ip][j][k], uut_thd[si][ip][j][k]);
                        //}
                    }
        */

#ifdef GOLD_FULL
        for (ip = 0; ip < 6; ip++)
            for (j = 0; j < 9; j++)
                for (k = 0; k < 2; k++) {
                    //sim_out << "    pzvl:" << uut_phzvl[ip][j] << std::endl;
                    if (uut_vl[ip][j][k] != 0) {
                        if (ip <= 1 && j < 3) { // ME11
                            sim_out << "    st:" << ip
                                    << "    ch:" << j
                                    << "    ph:" << uut_ph[ip][j][k]
                                    << "    th:" << uut_th11[ip][j][k*2]
                                    << "    phzvl:" << uut_phzvl[ip][j]
                                    << endl;
                             //ph_high_prec = uut_ph[ip][j][k];
                        } else if (ip == 5 && j == 0) { // ME11 neighbor
                            sim_out << "    st:" << ip
                                    << "    ch:" << j
                                    << "    ph:" << uut_ph[ip][j][k]
                                    << "    th:" << uut_th11[2][0][k*2]
                                    << "    phzvl:" << uut_phzvl[ip][j]
                                    << endl;
                        } else {
                            sim_out << "    st:" << ip
                                    << "    ch:" << j
                                    << "    ph:" << uut_ph[ip][j][k]
                                    << "    th:" << uut_th[ip][j][k]
                                    << "    phzvl:" << uut_phzvl[ip][j]
                                    << endl;
                                    //<< " " << << " ph_zone: " << << endl;
                                    //uut_ph_hit[ip][j][k],uut_ph_zone[ip][j][k]); ph_high_prec = uut_ph[ip][j][k];
                        }
                    }
                }
#endif // GOLD_FULL
        // for (ip = 0; ip < 6; ip = ip+1)
//           if (uut_ph_zone[0][1] != 0)
//           {
//             //$fwrite(sim_out, "%b\n", uut_ph_zone[2][1]);
//              for (si = 0; si < ph_raw_w; si = si+1)
//                {
//                   if (uut_ph_zone[0][1][si] == 1)
//                     {
//                        ph_low_prec = si*32;
//                        ph_zone_phi = si;
//                     }
//                }
//              $fwrite (sim_out, "ph_zone_phi: %d ph_low_prec: %d ph_high_prec: %d ph_high_low_diff: %d\n",
//                       ph_zone_phi, ph_low_prec, ph_high_prec, ph_high_prec - ph_low_prec);
//
//           }
//            for (zi = 4; zi >= 1; zi = zi-1) // zone loop
//            {
//                $fwrite(sim_out, "zone: %d\n", zi);
//                for (si = 4; si >= 1; si = si-1) // station loop (4 stations only here)
//                {
//                    $fwrite(sim_out, "%b\n", uut_ph_ext[zi-1][si]);
//                }
//            }
//           $fwrite (sim_out, "q86: %b st1: %b st2: %b st3: %b st4: %b bx: %b \n",
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.qcode_best,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.st1,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.st2,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.st3,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.st4,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[86].php.bx
//                    );
//
//            $fwrite (sim_out, "q92: %b st1: %b st2: %b st3: %b st4: %b bx: %b \n",
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.qcode_best,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.st1,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.st2,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.st3,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.st4,
//                    uut_phps.gb.ph_pat_zone[0].ph_pat_hit[92].php.bx
//                    );
        /*
        for (iz = 0; iz < 4; iz++)
            for (ist = ph_raw_w; ist > 0; ist++) {
                //if (uut_ph_rank[iz][ist-1] > 0)
                    //$fwrite(sim_out, "pattern: z: %d ph: %d q: %h ly: %b%b%b str: %b%b%b\n",
                           //iz, ist, uut_ph_rank[iz][ist-1],
                           //uut_ph_rank[iz][ist-1][4], uut_ph_rank[iz][ist-1][2], uut_ph_rank[iz][ist-1][0],
                           //uut_ph_rank[iz][ist-1][5], uut_ph_rank[iz][ist-1][3], uut_ph_rank[iz][ist-1][1]
                           //);
            }
        */
        // for (iz = 0; iz < 4; iz = iz+1)

        /*
        for (iz = 0; iz < 4; iz++)
            for (ir = 0; ir < 3; ir++) {
                //if (uut_ph_q[iz][ir] > 0)
                    //$fwrite(sim_out, "pattern on match input: z: %d r: %d ph_num: %d ph_q: %h ly: %b%b%b str: %b%b%b\n",
                    //iz, ir, uut_ph_num[iz][ir], uut_ph_q[iz][ir],
                    //uut_ph_q[iz][ir][4], uut_ph_q[iz][ir][2], uut_ph_q[iz][ir][0],
                    //uut_ph_q[iz][ir][5], uut_ph_q[iz][ir][3], uut_ph_q[iz][ir][1]
                    //);
            }
        */
        // for (iz = 0; iz < 4; iz = iz+1)

//                 $fwrite (sim_out, "csc ME2 ch 5: vl: %d th: %d cpat: %d\n",
//                          uut_cdl.vlo_csc[2][4][0], uut_cdl.tho_csc[2][4][0], uut_cdl.cpato_csc[2][4][0]);
//                 $fwrite (sim_out, "csc ME4 ch 5: vl: %d th: %d cpat: %d\n",
//                          uut_cdl.vlo_csc[4][4][0], uut_cdl.tho_csc[4][4][0], uut_cdl.cpato_csc[4][4][0]);

//                 $fwrite (sim_out, "dl0 ME2 ch 5: vl: %d th: %d cpat: %d\n",
//                          uut_cdl.vlo[0][2][4][0], uut_cdl.tho[0][2][4][0], uut_cdl.cpato[0][2][4][0]);
//                 $fwrite (sim_out, "dl0 ME4 ch 5: vl: %d th: %d cpat: %d\n",
//                          uut_cdl.vlo[0][4][4][0], uut_cdl.tho[0][4][4][0], uut_cdl.cpato[0][4][4][0]);
//           $fwrite(sim_out, "ph_rank: ");
//            for (iz = 0; iz < 4; iz = iz+1)
//            {
//                for (ir = 0; ir < 3; ir = ir+1)
//                  {
//                     $fwrite(sim_out, "%b ", uut_ph_rank[iz][ir]);
//                  }
//            }
//           $fwrite(sim_out, "\n");
//
//           $fwrite(sim_out, "valid: %b %b %b %d\n",
//                   uut_srts.gb.ph_zone[0].zb3.wini[0],
//                   uut_srts.gb.ph_zone[0].zb3.wini[1],
//                   uut_srts.gb.ph_zone[0].zb3.wini[2],
//                   cnrex
//                   );
//            for (iz = 0; iz < uut_mphseg.fs_loop[0].fs_00.tot_diff; iz = iz+1)
//            {
////                      for (ip = 0; ip < 3; ip = ip+1)
//                {
//                    if (uut_mphseg.fs_loop[0].fs_00.ph_diff[iz] != 511)
//                        $fwrite(sim_out, "iz: %d ip: %d fs_00 ph_diff: %d\n", iz, ip, uut_mphseg.fs_loop[0].fs_00.ph_diff[iz]);
//
//                    if (uut_mphseg.fs_loop[0].fs_01.ph_diff[iz] != 31)
//                        $fwrite(sim_out, "iz: %d ip: %d fs_01 ph_diff: %d\n", iz, ip, uut_mphseg.fs_loop[0].fs_01.ph_diff[iz]);
//
//                    if (uut_mphseg.fs_loop[0].fs_02.ph_diff[iz] != 255)
//                        $fwrite(sim_out, "iz: %d ip: %d fs_02 ph_diff: %d\n", iz, ip, uut_mphseg.fs_loop[0].fs_02.ph_diff[iz]);
//
//                    if (uut_mphseg.fs_loop[0].fs_03.ph_diff[iz] != 255)
//                        $fwrite(sim_out, "iz: %d ip: %d fs_03 ph_diff: %d\n", iz, ip, uut_mphseg.fs_loop[0].fs_03.ph_diff[iz]);
//                }
//            }
//           if (uut_mphseg.fs_loop[0].fs_00.vid != 2'b0)
//             $fwrite (sim_out, "fs_00 ph_match: %d\n", uut_mphseg.fs_loop[0].fs_00.ph_match);
//           if (uut_mphseg.fs_loop[0].fs_01.vid != 2'b0)
//             $fwrite (sim_out, "fs_01 ph_match: %d\n", uut_mphseg.fs_loop[0].fs_01.ph_match);
//           if (uut_mphseg.fs_loop[0].fs_02.vid != 2'b0)
//             $fwrite (sim_out, "fs_02 ph_match: %d\n", uut_mphseg.fs_loop[0].fs_02.ph_match);
//           if (uut_mphseg.fs_loop[0].fs_03.vid != 2'b0)
//             $fwrite (sim_out, "fs_03 ph_match: %d\n", uut_mphseg.fs_loop[0].fs_03.ph_match);
//
//            for (ibx = 0; ibx < max_drift; ibx = ibx+1) // history
//                for (ist = 0; ist < 5; ist = ist+1) // station
//                    for (ich = 0; ich < 9; ich = ich+1) // chamber
//                        for (isg = 0; isg < 2; isg = isg+1) // segment
//                            if (uut_vld[ibx][ist][ich][isg]) $fwrite(sim_out, "delayed stub: bx:%d st:%d ch:%d sg:%d  ph: %d\n",
//                                                                     ibx,ist,ich,isg, uut_phd[ibx][ist][ich][isg]);
        /*
        for (iz = 0; iz < 4; iz++) // zone loop
            for (ip = 0; ip < 3; ip++) // best pattern number
                for (ist = 0; ist < 4; ist++) { // station
                    //if (uut_patt_ph_vi[iz][ip][ist] > 0) {
//                                  if (iz < 2 && ist == 0)
//                                      $fwrite(sim_out, "match seg: z: %d pat: %d st: %d   vi: %b hi: %d ci: %d si: %d ph: %d  th11: %d %d %d %d\n",
//                                             iz, ip, ist, uut_patt_ph_vi[iz][ip][ist], uut_patt_ph_hi[iz][ip][ist], uut_patt_ph_ci[iz][ip][ist],
//                                              uut_patt_ph_si[iz][ip][ist], uut_ph_match[iz][ip][ist],
//                                             uut_th_match11[iz][ip][0], uut_th_match11[iz][ip][1],
//                                              uut_th_match11[iz][ip][2], uut_th_match11[iz][ip][3]);
//                                  else
                            //$fwrite(sim_out, "match seg: z: %d pat: %d st: %d   vi: %b hi: %d ci: %d si: %d ph: %d  th  : %d %d\n",
                                   //iz, ip, ist, uut_patt_ph_vi[iz][ip][ist], uut_patt_ph_hi[iz][ip][ist], uut_patt_ph_ci[iz][ip][ist],
                                    //uut_patt_ph_si[iz][ip][ist], uut_ph_match[iz][ip][ist],
                                    //uut_th_match[iz][ip][ist][0], uut_th_match[iz][ip][ist][1]);

                    //}
                } // for (ist = 0; ist < 4; ist = ist + 1)
        */
        // for (iz = 0; iz < 4; iz = iz+1)


        /*
        for (ip = 0; ip < 6; ip++)
            for (j = 0; j < 9; j++)
                for (k = 0; k < 2; k++) {
                    //if (uut_twom.vlo[0][ip][j][k] != 0) {
//                                  $fwrite(sim_out, "stub: st: %d ch: %d ql: %d wg: %d hs: %d\n",
//                                          ip, j, ql[ip][j][k], wg[ip][j][k], hs[ip][j][k]);
                        //$fwrite(sim_out, "2mu stub: st: %d ch: %d stub: %d\n",
                                //ip, j, k);
                    //}
                }
        */
        // for (ip = 0; ip < 6; ip = ip+1)

        //if (uut_twom.bt_rank_i[0] != 0) {
            //$fwrite (sim_out, "2mu track: rank: %d\n", uut_twom.bt_rank_i[0]);
        //}

       //if ( uut_ds.gb.zl11[0].pl[0].da.bd12.dth[0] != 0) {
//                $fwrite(sim_out, "ds: vi[0]: %b vi[1]: %b\n" ,
//                   uut_ds.vi[0][0][0],
//                   uut_ds.vi[0][0][1]
//                   );
//
//                $fwrite(sim_out, "da: vi[0]: %b vi[1]: %b\n" ,
//                   uut_ds.gb.zl11[0].pl[0].da.vi[0],
//                   uut_ds.gb.zl11[0].pl[0].da.vi[1]
//                   );
//
//                $fwrite(sim_out, "dth:  %d %d %d %d %d %d %d %d dvl: %b\n" ,
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[0],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[1],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[2],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[3],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[4],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[5],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[6],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dth[7],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.dvl
//                   );
//
//                $fwrite(sim_out, "cmp1: %d %d %d %d\n" ,
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp1[0],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp1[1],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp1[2],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp1[3]
//                   );
//                $fwrite(sim_out, "cmp2: %d %d\n" ,
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp2[0],
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.cmp2[1]
//                   );
//                $fwrite(sim_out, "bth:  %d\n" ,
//                   uut_ds.gb.zl11[0].pl[0].da.bd12.bth,
//                   );
         //}


//                    if (uut_mphseg.ph_match[0][0][1] != 4'hf)
//                        $fwrite (sim_out, "segment match: ph: %d\n", uut_mphseg.ph_match[0][0][1]);



//            for (ip = 0; ip < 3; ip = ip+1) // best track
//              {
//
//                $fwrite (sim_out, "best_track_i: %d bt_stA: %d chA: %d\n", ip, uut_pta.bt_stA[ip], uut_pta.chA[ip]);
//            }
//           $fwrite (sim_out, "stA: %d stB: %d chA: %d, chB: %d\n", uut_pta.stA, uut_pta.stB, uut_pta.chA, uut_pta.chB);

       /*
       for (iz = 0; iz < 4; iz++) // zone loop
         for (ip = 0; ip < 3; ip++) // pattern loop
            for (is = 0; is < 5; is++) { // station loop
                 //if (uut_bt.cn_vi[iz][ip][is] != 0)
                   //$fwrite (sim_out, "bt zone: %d pat: %d station: %d vi: %d ci: %d si: %d hi: %d cand_bx: %d\n",
                            //iz, ip, is,
                            //uut_bt.cn_vi[iz][ip][is],uut_bt.cn_ci[iz][ip][is], uut_bt.cn_si[iz][ip][is], uut_bt.cn_hi[iz][ip][is],
                            //uut_bt.cand_bx[iz][ip]
                            //);
            }
        */
       // for (iz = 0; iz < 4; iz = iz+1)

       //if (uut_bt.exists != 0) $fwrite (sim_out, "exists: %b_%b_%b\n", uut_bt.exists[35:24], uut_bt.exists[23:12], uut_bt.exists[11:0]);
       //if (uut_bt.kill1  != 0) $fwrite (sim_out, "kill1:  %b_%b_%b\n", uut_bt.kill1[35:24], uut_bt.kill1[23:12], uut_bt.kill1[11:0]);

       //if (uut_ds.gb.zl11[0].pl[0].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 0, 0, uut_ds.gb.zl11[0].pl[0].da.vstat);
       //if (uut_ds.gb.zl  [2].pl[0].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 2, 0, uut_ds.gb.zl  [2].pl[0].da.vstat);
       //if (uut_ds.gb.zl11[0].pl[1].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 0, 1, uut_ds.gb.zl11[0].pl[1].da.vstat);
       //if (uut_ds.gb.zl  [2].pl[1].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 2, 1, uut_ds.gb.zl  [2].pl[1].da.vstat);
       //if (uut_ds.gb.zl11[0].pl[2].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 0, 2, uut_ds.gb.zl11[0].pl[2].da.vstat);
       //if (uut_ds.gb.zl  [2].pl[2].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 2, 2, uut_ds.gb.zl  [2].pl[2].da.vstat);
       //if (uut_ds.gb.zl11[1].pl[0].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 1, 0, uut_ds.gb.zl11[1].pl[0].da.vstat);
       //if (uut_ds.gb.zl  [3].pl[0].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 3, 0, uut_ds.gb.zl  [3].pl[0].da.vstat);
       //if (uut_ds.gb.zl11[1].pl[1].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 1, 1, uut_ds.gb.zl11[1].pl[1].da.vstat);
       //if (uut_ds.gb.zl  [3].pl[1].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 3, 1, uut_ds.gb.zl  [3].pl[1].da.vstat);
       //if (uut_ds.gb.zl11[1].pl[2].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 1, 2, uut_ds.gb.zl11[1].pl[2].da.vstat);
       //if (uut_ds.gb.zl  [3].pl[2].da.vstat != 0) $fwrite (sim_out, "pre  delta vsat[%d][%d] = %b\n", 3, 2, uut_ds.gb.zl  [3].pl[2].da.vstat);
//           for (iz = 0; iz < 4; iz = iz+1) // zone loop
//           {
//               for (ip = 0; ip < 3; ip = ip+1) // pattern loop
//               {
//                    if (uut_ph_qr[iz][ip] != 0)
//                      $fwrite (sim_out, "pre  delta rank[%d][%d] = %b\n", iz, ip, uut_ph_qr[iz][ip]);
//                    if (uut_rank[iz][ip] != 0)
//                      $fwrite (sim_out, "post delta rank[%d][%d] = %b\n", iz, ip, uut_rank[iz][ip]);
//               }
//           }
//
//            for (iz = 0; iz < 4; iz = iz+1)
//            {
//                for (ip = 0; ip < 3; ip = ip+1)
//                {
////                          if (uut_rank[iz][ip] != 0)
//                        $fwrite(sim_out, "z: %d pat: %d ph_deltas: %d %d %d %d %d %d th_deltas: %d %d %d %d %d %d th_match: %d %d %d %d\n",
//                                iz, ip,
//                                uut_delta_ph[iz][ip][0], uut_delta_ph[iz][ip][1], uut_delta_ph[iz][ip][2], uut_delta_ph[iz][ip][3], uut_delta_ph[iz][ip][4], uut_delta_ph[iz][ip][5],
//                                uut_delta_th[iz][ip][0], uut_delta_th[iz][ip][1], uut_delta_th[iz][ip][2], uut_delta_th[iz][ip][3], uut_delta_th[iz][ip][4], uut_delta_th[iz][ip][5],
//                                uut_th_match[iz][ip][0][0], uut_th_match[iz][ip][1][0], uut_th_match[iz][ip][2][0], uut_th_match[iz][ip][3][0]
//                                );
//                }
//            }
        /*
        for (ip = 0; ip < 3; ip++) {
            //if (uut_ptlut_addr_val[0]) {
                // get PT from table
                //pt_word = ptlut[uut_ptlut_addr[ip]/2]; // pt_word contains two pt values
                //pt_word_rev = brev(pt_word); // bytes are reversed for some reason, unreverse
                //pt_value = uut_ptlut_addr[ip][0] == 1'b1 ?
                           //pt_word_rev[17:9] : pt_word_rev[8:0]; // low bit of address selects value
                //$fwrite(sim_out, " track pt address: %b %h pt: %h\n", uut_ptlut_addr[ip], uut_ptlut_addr[ip], pt_value);

                //$fwrite (sim_out, "m15_theta: %d m15_r2_rpc: %d clct_2b_A: %d clct_is_0: %d n_rpc: %d m15_r1_rpc: %d m15_r1_rpc_clct1: %d m15_theta_rem3: %d ring1_2: %d\n",
                         //uut_pta.m15_theta[ip], uut_pta.m15_r2_rpc[ip], uut_pta.clct_2b_A[ip], uut_pta.clct_is_0[ip], uut_pta.n_rpc[ip],
                         //uut_pta.m15_r1_rpc[ip], uut_pta.m15_r1_rpc_clct1[ip], uut_pta.m15_theta_rem3[ip], uut_pta.ring1_2[ip]);

                //$fwrite (sim_out, "bt_theta: %d theta_7b: %d theta_5b: %d\n",
                         //uut_pta.bt_theta[ip], uut_pta.theta_7b[ip], uut_pta.theta_5b[ip]);

                //$fwrite(best_tracks, "ev: %d track pt address: %h pt_word: %h pt: %h\n",
                       //iev, uut_ptlut_addr[ip], pt_word_rev, pt_value);

            //}

            //if (uut_ptlut_addr_val[ip]) {
               //$fwrite(sim_out, " GMT charge: %d\n", uut_gmt_crg[ip]);
            //}


            if (bt_rank[ip] != 0) {
                //$fwrite(sim_out, "i: %d sign_th: %d %d %d %d %d %d delta_th: %d %d %d %d %d %d dth_red: %d\n",
                        //ip,
                        //uut_pta.bt_sign_th[ip][0], uut_pta.bt_sign_th[ip][1], uut_pta.bt_sign_th[ip][2], uut_pta.bt_sign_th[ip][3], uut_pta.bt_sign_th[ip][4], uut_pta.bt_sign_th[ip][5],
                        //uut_pta.bt_delta_th[ip][0], uut_pta.bt_delta_th[ip][1], uut_pta.bt_delta_th[ip][2], uut_pta.bt_delta_th[ip][3], uut_pta.bt_delta_th[ip][4], uut_pta.bt_delta_th[ip][5],
                        //uut_pta.dth_red[ip]
                        //);
                //$fwrite(sim_out, "ME ");
                //if (bt_rank[ip][5]) $fwrite(sim_out, "-1");
                //if (bt_rank[ip][3]) $fwrite(sim_out, "-2");
                //if (bt_rank[ip][1]) $fwrite(sim_out, "-3");
                //if (bt_rank[ip][0]) $fwrite(sim_out, "-4");
                //$fwrite(sim_out, " track: %d  rank: %h ly: %d ph_deltas: %d %d %d %d %d %d th_deltas: %d %d %d %d %d %d phi: %d,  theta: %d cpat: %d\n",
                        //ip, bt_rank[ip],
                        //{bt_rank[ip][5], bt_rank[ip][3], bt_rank[ip][1], bt_rank[ip][0]},
                        //bt_delta_ph[ip][0], bt_delta_ph[ip][1], bt_delta_ph[ip][2], bt_delta_ph[ip][3], bt_delta_ph[ip][4], bt_delta_ph[ip][5],
                        //bt_delta_th[ip][0], bt_delta_th[ip][1], bt_delta_th[ip][2], bt_delta_th[ip][3], bt_delta_th[ip][4], bt_delta_th[ip][5],
                        //bt_phi[ip], bt_theta[ip], bt_cpattern[ip][0]);

                //$fwrite(best_tracks, "ev: %d ME ", iev);
                //if (bt_rank[ip][5]) $fwrite(best_tracks, "-1"); else $fwrite(best_tracks, "  ");
                //if (bt_rank[ip][3]) $fwrite(best_tracks, "-2"); else $fwrite(best_tracks, "  ");
                //if (bt_rank[ip][1]) $fwrite(best_tracks, "-3"); else $fwrite(best_tracks, "  ");
                //if (bt_rank[ip][0]) $fwrite(best_tracks, "-4"); else $fwrite(best_tracks, "  ");
                //$fwrite(best_tracks, " track: %d  rank: %h ly: %d ph_deltas: %d %d %d %d %d %d th_deltas: %d %d %d %d %d %d phi: %d,  theta: %d cpat: %d hcs:",
                        //ip, bt_rank[ip],
                        //{bt_rank[ip][5], bt_rank[ip][3], bt_rank[ip][1], bt_rank[ip][0]},
                        //bt_delta_ph[ip][0], bt_delta_ph[ip][1], bt_delta_ph[ip][2], bt_delta_ph[ip][3], bt_delta_ph[ip][4], bt_delta_ph[ip][5],
                        //bt_delta_th[ip][0], bt_delta_th[ip][1], bt_delta_th[ip][2], bt_delta_th[ip][3], bt_delta_th[ip][4], bt_delta_th[ip][5],
                        //bt_phi[ip], bt_theta[ip], bt_cpattern[ip][0]);
                for (j = 0; j < 5; j++) {
                    //if (bt_vi[ip][j] == 1'b1)
                        //$fwrite(best_tracks, " %d%h%d", bt_hi[ip][j], bt_ci[ip][j], bt_si[ip][j]);
                    //else
                        //$fwrite(best_tracks, " ---");
                }
               //$fwrite (best_tracks, "\n");

                //$fwrite(best_tracks_short, "ev: %d ", iev);
                //$fwrite(best_tracks_short, " track: %d rank: %h phi: %d theta: %d\n",
                        //ip, bt_rank[ip], bt_phi[ip], bt_theta[ip]);
            }

            for (j = 0; j < 5; j++) {
              //if (bt_vi[ip][j] == 1'b1)
                //$fwrite(sim_out, "track segments: st: %d v: %d h: %d c: %d s: %d\n", j, bt_vi[ip][j], bt_hi[ip][j], bt_ci[ip][j], bt_si[ip][j]);
            }
//                        $fwrite(sim_out, "\n");

//                      if (gmt_eta[ip] != 239)
//                        $fwrite (sim_out, " gmt_phi: %d, gmt_eta: %d\n", gmt_phi[ip], gmt_eta[ip]);
        } // for (ip = 0; ip < 3; ip = ip+1)
        */

        /*
        for (gz = 0; gz < 4; gz++) // zone loop
            for (gn = 0; gn < 3; gn++) { // pattern number
                //if (uut_rank[gz][gn] > 0)
                    //$fwrite(sim_out, "cand. rank: z: %d pat: %d rank: %h\n", gz, gn, uut_rank[gz][gn]);

                for (gs = 0; gs < 5; gs++) { // station
                  //if (uut_bt.cn_vi_w[gz][gn][gs] == 1'b1)
                      //$fwrite(sim_out, "cand. segments: zone: %d pat: %d st: %d v: %d h: %d c: %d s: %d\n",
                      //gz, gn, gs,
                      //uut_bt.cn_vi_w[gz][gn][gs],
                      //uut_bt.cn_hi_w[gz][gn][gs],
                      //uut_bt.cn_ci_w[gz][gn][gs],
                      //uut_bt.cn_si_w[gz][gn][gs]);
                }
            }
        */

//            $fwrite(sim_out, "th zones:\n");
//            for (iz = 0; iz < 6; iz = iz+1) {
//                for (ist = 1; ist <= 4; ist = ist + 1) {
//                $fwrite(sim_out, "%h\n", uut_th_ext[iz][ist]);
//                }
//                $fwrite(sim_out, "\n");
//            }
//                  $fwrite(sim_out, "th_hito[2][8]: %h\n", uut_th_hito[2][8]);

//            $fwrite(sim_out, "th zones padded:\n");
//            for (iz = 0; iz < 6; iz = iz+1) {
//                $fwrite(sim_out, "%b\n", uut_thps.stp[iz][0]);
//                $fwrite(sim_out, "   %b\n", uut_thps.st [iz][2]);
//                $fwrite(sim_out, "%b\n", uut_thps.stp[iz][1]);
//                $fwrite(sim_out, "%b\n", uut_thps.stp[iz][2]);
//                $fwrite(sim_out, "\n");
//            }
//
//
//            $fwrite(sim_out, "%b\n   %b\n%b\n%b     %d %d %b\n",
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.p1,
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.st2,
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.p3,
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.p4,
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.bx[0][0],
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.bx[1][0],
//            uut_thps.gb.th_pat_zone[2].th_pat_hit[49].thp.lyhits);
        //              $fwrite(sim_out, "bx: %d drifftime: %d\n", uut_phps.gb.ph_pat_zone[0].ph_pat_hit[12].php.bx[1][0], uut_phps.gb.ph_pat_zone[0].ph_pat_hit[12].php.drifttime);
    }

    //$fwrite(sim_out, "elapsed_time: %t\n", elapsed_time);
    in               .close();
    best_tracks      .close();
    sim_out          .close();
    best_tracks_short.close();

#if defined(GOLD) || defined(GOLD_FULL)
    sim_out << "All elements match reference!" << endl;
    cout    << "All elements match reference!" << endl;
    gold_file.close();
#endif

    return 0;
}

