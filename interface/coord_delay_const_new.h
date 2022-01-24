#ifndef _COORD_DELAY_CONST_
#define _COORD_DELAY_CONST_

// Definitions
/// Delay
#define DELAY_true_0 4 // Delay on time_i to time_o & q_i to q_o
#define DELAY_true_1 0 // Delay on Everything Else
#define DELAY_0 (DELAY_true_0+2)
#define DELAY_1 (DELAY_true_1+2)

/// Delay Checking
#ifndef __SYNTHESIS__
	static_assert(DELAY_0 >= 2, "coord_delay - DELAY_0 must be greater than or equal to 2");
	static_assert(DELAY_1 >= 2, "coord_delay - DELAY_1 must be greater than or equal to 2");
#endif // Delay Checking

/// BX Config
#define COORD_max_drift 1 // TODO: only supports 1 right now

/// Structural Defines
#define __FULL_OUT__ // Change Between Output and Struct values
#define __ENABLE_DLY_BYPASS__ // Enable bypassing delays while simulating


// Constants
/// IO Dimensiosn
//// time Dimensions
const int DIM_time_0 = 6;
const int DIM_time_1 = 9;
const int DIM_time_2 = seg_ch;
//// vl Dimensions
const int DIM_vl_0   = 12;
const int DIM_vl_1   = 9;
//// q Dimensions
const int DIM_q_0 = 12;
const int DIM_q_1 = 9;
const int DIM_q_2 = seg_ch;
//// cpatr Dimensions
const int DIM_cpatr_0 = 6;
const int DIM_cpatr_1 = 9;
const int DIM_cpatr_2 = seg_ch;
//// ph Dimensions
const int DIM_ph_0 = 12;
const int DIM_ph_1 = 9;
const int DIM_ph_2 = seg_ch;
//// th11 Dimensions
const int DIM_th11_0 = 3;
const int DIM_th11_1 = 3;
const int DIM_th11_2 = th_ch11;
//// th Dimensions
const int DIM_th_0 = 12;
const int DIM_th_1 = 9;
const int DIM_th_2 = seg_ch;
//// phzvl Dimensions
const int DIM_phzvl_0 = 12;
const int DIM_phzvl_1 = 9;
//// ph_hito Dimensions
const int DIM_ph_hito_0 = 13;
const int DIM_ph_hito_1 = 9;

/// IO ap uint widths
const int WDH_time = 6;
const int WDH_vl = seg_ch;
const int WDH_q = 4;
const int WDH_cpatr = 4;
const int WDH_ph = bw_fph;
const int WDH_th11 = bw_th;
const int WDH_th = bw_th;
const int WDH_phzvl = 7;
const int WDH_ph_hito = ph_hit_w;

/// IO Dimension Arrays (TODO: (low priority) remove need in coord_delay.cc)
const int DIM_time[] = 		{ DIM_time_0,    DIM_time_1,  DIM_time_2 };
const int DIM_vl[] = 		{ DIM_vl_0,      DIM_vl_1 };
const int DIM_q[] = 		{ DIM_q_0,       DIM_q_1,     DIM_q_2 };
const int DIM_cpatr[] = 	{ DIM_cpatr_0,   DIM_cpatr_1, DIM_cpatr_2 };
const int DIM_ph[] = 		{ DIM_ph_0,      DIM_ph_1,    DIM_ph_2 };
const int DIM_th11[] = 		{ DIM_th11_0,    DIM_th11_1,  DIM_th11_2 };
const int DIM_th[] = 		{ DIM_th_0,      DIM_th_1,    DIM_th_2 };
const int DIM_phzvl[] = 	{ DIM_phzvl_0,   DIM_phzvl_1 };
const int DIM_ph_hito[] = 	{ DIM_ph_hito_0, DIM_ph_hito_1 };

#endif // _COORD_DELAY_CONST_