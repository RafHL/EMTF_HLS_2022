// Given by Alex Madorsky
// Translated by Rafael Hernandez into C++
// Purposefully not called fill_params.cpp to allow g++ folder/*.cpp

ap_uint<4>        th_corr_mem[3][3][th_corr_mem_sz]; // Will get filled here
ap_uint<6>        th_mem     [6][9][th_mem_sz];      // Will get filled here
ap_uint<13>       params     [6][9][6];              // Will get filled here
#define DISP_INIT_SIZE 61
ap_uint<bw_th>    th_init    [DISP_INIT_SIZE]; // chamber origins in th
ap_uint<bw_ph+1>  ph_disp    [DISP_INIT_SIZE]; // chamber displacements in ph
ap_uint<bw_th>    th_disp    [DISP_INIT_SIZE]; // chamber displacements in th
#define PH_INITX_SIZE 16
ap_uint<bw_fph+1> ph_init0   [PH_INITX_SIZE];
ap_uint<bw_fph+1> ph_init1   [PH_INITX_SIZE];
ap_uint<bw_fph+1> ph_init2   [PH_INITX_SIZE];
ap_uint<bw_fph+1> ph_init3   [PH_INITX_SIZE];
ap_uint<bw_fph+1> ph_init4   [PH_INITX_SIZE];

// fill th LUTs -- array[station][chamber]
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_1_st_1_ch_1.lut" , th_corr_mem[0][0], th_corr_mem_sz); // pc11
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_1_st_1_ch_2.lut" , th_corr_mem[0][1], th_corr_mem_sz); // pc11
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_1_st_1_ch_3.lut" , th_corr_mem[0][2], th_corr_mem_sz); // pc11
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_1_st_1_ch_13.lut", th_corr_mem[2][0], th_corr_mem_sz); // pcn11

file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_2_st_1_ch_1.lut" , th_corr_mem[1][0], th_corr_mem_sz); // pc11
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_2_st_1_ch_2.lut" , th_corr_mem[1][1], th_corr_mem_sz); // pc11
file2arrayu<4>(dpath + "/vl_lut/vl_th_corr_lut" + fes + "sub_2_st_1_ch_3.lut" , th_corr_mem[1][2], th_corr_mem_sz); // pc11

file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_1.lut" , th_mem     [0][0], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_2.lut" , th_mem     [0][1], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_3.lut" , th_mem     [0][2], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_4.lut" , th_mem     [0][3], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_5.lut" , th_mem     [0][4], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_6.lut" , th_mem     [0][5], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_7.lut" , th_mem     [0][6], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_8.lut" , th_mem     [0][7], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_9.lut" , th_mem     [0][8], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_13.lut", th_mem     [5][0], th_mem_sz     ); // pcn11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_14.lut", th_mem     [5][1], th_mem_sz     ); // pcn
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_1_st_1_ch_15.lut", th_mem     [5][2], th_mem_sz     ); // pcn

file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_1.lut" , th_mem     [1][0], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_2.lut" , th_mem     [1][1], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_3.lut" , th_mem     [1][2], th_mem_sz     ); // pc11
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_4.lut" , th_mem     [1][3], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_5.lut" , th_mem     [1][4], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_6.lut" , th_mem     [1][5], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_7.lut" , th_mem     [1][6], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_8.lut" , th_mem     [1][7], th_mem_sz     ); // pc12
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "sub_2_st_1_ch_9.lut" , th_mem     [1][8], th_mem_sz     ); // pc12

file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_1.lut"       , th_mem     [2][0], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_2.lut"       , th_mem     [2][1], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_3.lut"       , th_mem     [2][2], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_4.lut"       , th_mem     [2][3], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_5.lut"       , th_mem     [2][4], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_6.lut"       , th_mem     [2][5], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_7.lut"       , th_mem     [2][6], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_8.lut"       , th_mem     [2][7], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_9.lut"       , th_mem     [2][8], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_10.lut"      , th_mem     [5][3], th_mem_sz     ); // pcn
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_2_ch_11.lut"      , th_mem     [5][4], th_mem_sz     ); // pcn

file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_1.lut"       , th_mem     [3][0], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_2.lut"       , th_mem     [3][1], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_3.lut"       , th_mem     [3][2], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_4.lut"       , th_mem     [3][3], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_5.lut"       , th_mem     [3][4], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_6.lut"       , th_mem     [3][5], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_7.lut"       , th_mem     [3][6], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_8.lut"       , th_mem     [3][7], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_9.lut"       , th_mem     [3][8], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_10.lut"      , th_mem     [5][5], th_mem_sz     ); // pcn
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_3_ch_11.lut"      , th_mem     [5][6], th_mem_sz     ); // pcn

file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_1.lut"       , th_mem     [4][0], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_2.lut"       , th_mem     [4][1], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_3.lut"       , th_mem     [4][2], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_4.lut"       , th_mem     [4][3], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_5.lut"       , th_mem     [4][4], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_6.lut"       , th_mem     [4][5], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_7.lut"       , th_mem     [4][6], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_8.lut"       , th_mem     [4][7], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_9.lut"       , th_mem     [4][8], th_mem_sz     ); // pc
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_10.lut"      , th_mem     [5][7], th_mem_sz     ); // pcn
file2arrayu<6>(dpath + "/vl_lut/vl_th_lut"      + fes + "st_4_ch_11.lut"      , th_mem     [5][8], th_mem_sz     ); // pcn

file2arrayu<bw_th  >(dpath + "/vl_lut/th_init" + fest + ".lut", th_init, DISP_INIT_SIZE);
file2arrayu<bw_ph+1>(dpath + "/vl_lut/ph_disp" + fest + ".lut", ph_disp, DISP_INIT_SIZE);
file2arrayu<bw_th  >(dpath + "/vl_lut/th_disp" + fest + ".lut", th_disp, DISP_INIT_SIZE);

file2arrayu<bw_fph+1>(dpath + "/vl_lut/ph_init_full" + fest + "_st_0.lut", ph_init0, PH_INITX_SIZE);
file2arrayu<bw_fph+1>(dpath + "/vl_lut/ph_init_full" + fest + "_st_1.lut", ph_init1, PH_INITX_SIZE);
file2arrayu<bw_fph+1>(dpath + "/vl_lut/ph_init_full" + fest + "_st_2.lut", ph_init2, PH_INITX_SIZE);
file2arrayu<bw_fph+1>(dpath + "/vl_lut/ph_init_full" + fest + "_st_3.lut", ph_init3, PH_INITX_SIZE);
file2arrayu<bw_fph+1>(dpath + "/vl_lut/ph_init_full" + fest + "_st_4.lut", ph_init4, PH_INITX_SIZE);


params[0][0][0] = ph_init0[0];   // ME11b pc11
params[0][0][1] = ph_disp [0];   // ME11b pc11
params[0][0][2] = ph_init0[0+9]; // ME11a pc11
params[0][0][3] = ph_disp [0+9]; // ME11a pc11
params[0][0][4] = th_init [0];   //       pc11
params[0][0][5] = th_disp [0];   //       pc11

params[0][1][0] = ph_init0[1];   // ME11b pc11
params[0][1][1] = ph_disp [1];   // ME11b pc11
params[0][1][2] = ph_init0[1+9]; // ME11a pc11
params[0][1][3] = ph_disp [1+9]; // ME11a pc11
params[0][1][4] = th_init [1];   //       pc11
params[0][1][5] = th_disp [1];   //       pc11

params[0][2][0] = ph_init0[2];   // ME11b pc11
params[0][2][1] = ph_disp [2];   // ME11b pc11
params[0][2][2] = ph_init0[2+9]; // ME11a pc11
params[0][2][3] = ph_disp [2+9]; // ME11a pc11
params[0][2][4] = th_init [2];   //       pc11
params[0][2][5] = th_disp [2];   //       pc11

params[0][3][0] = ph_init0[3];   // pc12
params[0][3][1] = th_init [3];   // pc12
params[0][3][2] = ph_disp [3];   // pc12
params[0][3][3] = th_disp [3];   // pc12

params[0][4][0] = ph_init0[4];   // pc12
params[0][4][1] = th_init [4];   // pc12
params[0][4][2] = ph_disp [4];   // pc12
params[0][4][3] = th_disp [4];   // pc12

params[0][5][0] = ph_init0[5];   // pc12
params[0][5][1] = th_init [5];   // pc12
params[0][5][2] = ph_disp [5];   // pc12
params[0][5][3] = th_disp [5];   // pc12

params[0][6][0] = ph_init0[6];   // pc12
params[0][6][1] = th_init [6];   // pc12
params[0][6][2] = ph_disp [6];   // pc12
params[0][6][3] = th_disp [6];   // pc12

params[0][7][0] = ph_init0[7];   // pc12
params[0][7][1] = th_init [7];   // pc12
params[0][7][2] = ph_disp [7];   // pc12
params[0][7][3] = th_disp [7];   // pc12

params[0][8][0] = ph_init0[8];   // pc12
params[0][8][1] = th_init [8];   // pc12
params[0][8][2] = ph_disp [8];   // pc12
params[0][8][3] = th_disp [8];   // pc12

params[1][0][0] = ph_init1[0];    // ME11b pc11
params[1][0][1] = ph_disp [16];   // ME11b pc11
params[1][0][2] = ph_init1[0+9];  // ME11a pc11
params[1][0][3] = ph_disp [16+9]; // ME11a pc11
params[1][0][4] = th_init [16];   //       pc11
params[1][0][5] = th_disp [16];   //       pc11

params[1][1][0] = ph_init1[1];    // ME11b pc11
params[1][1][1] = ph_disp [17];   // ME11b pc11
params[1][1][2] = ph_init1[1+9];  // ME11a pc11
params[1][1][3] = ph_disp [17+9]; // ME11a pc11
params[1][1][4] = th_init [17];   //       pc11
params[1][1][5] = th_disp [17];   //       pc11

params[1][2][0] = ph_init1[2];    // ME11b pc11
params[1][2][1] = ph_disp [18];   // ME11b pc11
params[1][2][2] = ph_init1[2+9];  // ME11a pc11
params[1][2][3] = ph_disp [18+9]; // ME11a pc11
params[1][2][4] = th_init [18];   //       pc11
params[1][2][5] = th_disp [18];   //       pc11

params[1][3][0] = ph_init1[ 3]; // pc12
params[1][3][1] = th_init [19]; // pc12
params[1][3][2] = ph_disp [19]; // pc12
params[1][3][3] = th_disp [19]; // pc12

params[1][4][0] = ph_init1[ 4]; // pc12
params[1][4][1] = th_init [20]; // pc12
params[1][4][2] = ph_disp [20]; // pc12
params[1][4][3] = th_disp [20]; // pc12

params[1][5][0] = ph_init1[ 5]; // pc12
params[1][5][1] = th_init [21]; // pc12
params[1][5][2] = ph_disp [21]; // pc12
params[1][5][3] = th_disp [21]; // pc12

params[1][6][0] = ph_init1[ 6]; // pc12
params[1][6][1] = th_init [22]; // pc12
params[1][6][2] = ph_disp [22]; // pc12
params[1][6][3] = th_disp [22]; // pc12

params[1][7][0] = ph_init1[ 7]; // pc12
params[1][7][1] = th_init [23]; // pc12
params[1][7][2] = ph_disp [23]; // pc12
params[1][7][3] = th_disp [23]; // pc12

params[1][8][0] = ph_init1[ 8]; // pc12
params[1][8][1] = th_init [24]; // pc12
params[1][8][2] = ph_disp [24]; // pc12
params[1][8][3] = th_disp [24]; // pc12

// neighbor sector ME1
// ME11 single chamber
params[5][0][0] = ph_init0[12]; // ME11b pcn11
params[5][0][1] = ph_disp [12]; // ME11b pcn11
params[5][0][2] = ph_init0[15]; // ME11a pcn11
params[5][0][3] = ph_disp [15]; // ME11a pcn11
params[5][0][4] = th_init [12]; //       pcn11
params[5][0][5] = th_disp [12]; //       pcn11

// rest of neighbor sector chambers in ME1
params[5][1][0] = ph_init0[1+12]; // pcn
params[5][1][1] = th_init [13];   // pcn
params[5][1][2] = ph_disp [13];   // pcn
params[5][1][3] = th_disp [13];   // pcn

params[5][2][0] = ph_init0[2+12]; // pcn
params[5][2][1] = th_init [14];   // pcn
params[5][2][2] = ph_disp [14];   // pcn
params[5][2][3] = th_disp [14];   // pcn

// ME234
params[2][0][0] = ph_init2[0];  // pc
params[2][0][1] = th_init [28]; // pc
params[2][0][2] = ph_disp [28]; // pc
params[2][0][3] = th_disp [28]; // pc

params[2][1][0] = ph_init2[1];  // pc
params[2][1][1] = th_init [29]; // pc
params[2][1][2] = ph_disp [29]; // pc
params[2][1][3] = th_disp [29]; // pc

params[2][2][0] = ph_init2[2];  // pc
params[2][2][1] = th_init [30]; // pc
params[2][2][2] = ph_disp [30]; // pc
params[2][2][3] = th_disp [30]; // pc

params[2][3][0] = ph_init2[3];  // pc
params[2][3][1] = th_init [31]; // pc
params[2][3][2] = ph_disp [31]; // pc
params[2][3][3] = th_disp [31]; // pc

params[2][4][0] = ph_init2[4];  // pc
params[2][4][1] = th_init [32]; // pc
params[2][4][2] = ph_disp [32]; // pc
params[2][4][3] = th_disp [32]; // pc

params[2][5][0] = ph_init2[5];  // pc
params[2][5][1] = th_init [33]; // pc
params[2][5][2] = ph_disp [33]; // pc
params[2][5][3] = th_disp [33]; // pc

params[2][6][0] = ph_init2[6];  // pc
params[2][6][1] = th_init [34]; // pc
params[2][6][2] = ph_disp [34]; // pc
params[2][6][3] = th_disp [34]; // pc

params[2][7][0] = ph_init2[7];  // pc
params[2][7][1] = th_init [35]; // pc
params[2][7][2] = ph_disp [35]; // pc
params[2][7][3] = th_disp [35]; // pc

params[2][8][0] = ph_init2[8];  // pc
params[2][8][1] = th_init [36]; // pc
params[2][8][2] = ph_disp [36]; // pc
params[2][8][3] = th_disp [36]; // pc

// ME234 neighbors
params[5][2*2-1+0][0] = ph_init2[0+9]; // pcn
params[5][2*2-1+0][1] = th_init [37];  // pcn
params[5][2*2-1+0][2] = ph_disp [37];  // pcn
params[5][2*2-1+0][3] = th_disp [37];  // pcn

params[5][2*2-1+1][0] = ph_init2[1+9]; // pcn
params[5][2*2-1+1][1] = th_init [38];  // pcn
params[5][2*2-1+1][2] = ph_disp [38];  // pcn
params[5][2*2-1+1][3] = th_disp [38];  // pcn


// ME234
params[3][0][0] = ph_init3[0];  // pc
params[3][0][1] = th_init [39]; // pc
params[3][0][2] = ph_disp [39]; // pc
params[3][0][3] = th_disp [39]; // pc

params[3][1][0] = ph_init3[1];  // pc
params[3][1][1] = th_init [40]; // pc
params[3][1][2] = ph_disp [40]; // pc
params[3][1][3] = th_disp [40]; // pc

params[3][2][0] = ph_init3[2];  // pc
params[3][2][1] = th_init [41]; // pc
params[3][2][2] = ph_disp [41]; // pc
params[3][2][3] = th_disp [41]; // pc

params[3][3][0] = ph_init3[3];  // pc
params[3][3][1] = th_init [42]; // pc
params[3][3][2] = ph_disp [42]; // pc
params[3][3][3] = th_disp [42]; // pc

params[3][4][0] = ph_init3[4];  // pc
params[3][4][1] = th_init [43]; // pc
params[3][4][2] = ph_disp [43]; // pc
params[3][4][3] = th_disp [43]; // pc

params[3][5][0] = ph_init3[5];  // pc
params[3][5][1] = th_init [44]; // pc
params[3][5][2] = ph_disp [44]; // pc
params[3][5][3] = th_disp [44]; // pc

params[3][6][0] = ph_init3[6];  // pc
params[3][6][1] = th_init [45]; // pc
params[3][6][2] = ph_disp [45]; // pc
params[3][6][3] = th_disp [45]; // pc

params[3][7][0] = ph_init3[7];  // pc
params[3][7][1] = th_init [46]; // pc
params[3][7][2] = ph_disp [46]; // pc
params[3][7][3] = th_disp [46]; // pc

params[3][8][0] = ph_init3[8];  // pc
params[3][8][1] = th_init [47]; // pc
params[3][8][2] = ph_disp [47]; // pc
params[3][8][3] = th_disp [47]; // pc

// ME234 neighbors
params[5][3*2-1+0][0] = ph_init3[0+9]; // pcn
params[5][3*2-1+0][1] = th_init [48];  // pcn
params[5][3*2-1+0][2] = ph_disp [48];  // pcn
params[5][3*2-1+0][3] = th_disp [48];  // pcn

params[5][3*2-1+1][0] = ph_init3[1+9]; // pcn
params[5][3*2-1+1][1] = th_init [49];  // pcn
params[5][3*2-1+1][2] = ph_disp [49];  // pcn
params[5][3*2-1+1][3] = th_disp [49];  // pcn


// ME234
params[4][0][0] = ph_init4[0];  // pc
params[4][0][1] = th_init [50]; // pc
params[4][0][2] = ph_disp [50]; // pc
params[4][0][3] = th_disp [50]; // pc

params[4][1][0] = ph_init4[1];  // pc
params[4][1][1] = th_init [51]; // pc
params[4][1][2] = ph_disp [51]; // pc
params[4][1][3] = th_disp [51]; // pc

params[4][2][0] = ph_init4[2];  // pc
params[4][2][1] = th_init [52]; // pc
params[4][2][2] = ph_disp [52]; // pc
params[4][2][3] = th_disp [52]; // pc

params[4][3][0] = ph_init4[3];  // pc
params[4][3][1] = th_init [53]; // pc
params[4][3][2] = ph_disp [53]; // pc
params[4][3][3] = th_disp [53]; // pc

params[4][4][0] = ph_init4[4];  // pc
params[4][4][1] = th_init [54]; // pc
params[4][4][2] = ph_disp [54]; // pc
params[4][4][3] = th_disp [54]; // pc

params[4][5][0] = ph_init4[5];  // pc
params[4][5][1] = th_init [55]; // pc
params[4][5][2] = ph_disp [55]; // pc
params[4][5][3] = th_disp [55]; // pc

params[4][6][0] = ph_init4[6];  // pc
params[4][6][1] = th_init [56]; // pc
params[4][6][2] = ph_disp [56]; // pc
params[4][6][3] = th_disp [56]; // pc

params[4][7][0] = ph_init4[7];  // pc
params[4][7][1] = th_init [57]; // pc
params[4][7][2] = ph_disp [57]; // pc
params[4][7][3] = th_disp [57]; // pc

params[4][8][0] = ph_init4[8];  // pc
params[4][8][1] = th_init [58]; // pc
params[4][8][2] = ph_disp [58]; // pc
params[4][8][3] = th_disp [58]; // pc

// ME234 neighbors
params[5][4*2-1+0][0] = ph_init4[0+9]; // pcn
params[5][4*2-1+0][1] = th_init [59];  // pcn
params[5][4*2-1+0][2] = ph_disp [59];  // pcn
params[5][4*2-1+0][3] = th_disp [59];  // pcn

params[5][4*2-1+1][0] = ph_init4[1+9]; // pcn
params[5][4*2-1+1][1] = th_init [60];  // pcn
params[5][4*2-1+1][2] = ph_disp [60];  // pcn
params[5][4*2-1+1][3] = th_disp [60];  // pcn

