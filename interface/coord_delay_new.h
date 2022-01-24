#ifndef _COORD_DELAY_
#define _COORD_DELAY_
#ifdef _LOCAL_AP_INT_H_
	#include "../include/ap_int.h"
#else
	#include <ap_int.h>
#endif
#include "../interface/spbits.h"
#include "../interface/coord_delay_const.h"

// Struct
struct coord_delay{
	void operator()	(
		// For legacy tb support
		#ifndef __FULL_OUT__
			ap_uint<WDH_time> 		(&time_i)	[DIM_time_0]   [DIM_time_1] [DIM_time_2],
			ap_uint<WDH_vl> 		(&vl_i)  	[DIM_vl_0]     [DIM_vl_1],
			ap_uint<WDH_q>			(&q_i)		[DIM_q_0]      [DIM_q_1]    [DIM_q_2],
			ap_uint<WDH_cpatr>		(&cpatr_i)	[DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2],
			ap_uint<WDH_ph>			(&ph_i)		[DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2],
			ap_uint<WDH_th11>		(&th11_i)	[DIM_th11_0]   [DIM_th11_1] [DIM_th11_2],
			ap_uint<WDH_th>			(&th_i)		[DIM_th_0]     [DIM_th_1]   [DIM_th_2],
			ap_uint<WDH_phzvl>		(&phzvl_i)	[DIM_phzvl_0]  [DIM_phzvl_1],
			ap_uint<WDH_ph_hito>	(&ph_hit_i)	[DIM_ph_hito_0][DIM_ph_hito_1]
		#endif // __FULL_OUT__

		// For Synthesis
		#ifdef __FULL_OUT__
			// Inputs
			const ap_uint<WDH_time> 	(&time_i)	[DIM_time_0]   [DIM_time_1] [DIM_time_2],
			const ap_uint<WDH_vl> 		(&vl_i)  	[DIM_vl_0]     [DIM_vl_1],
			const ap_uint<WDH_q>		(&q_i)		[DIM_q_0]      [DIM_q_1]    [DIM_q_2],
			const ap_uint<WDH_cpatr>	(&cpatr_i)	[DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2],
			const ap_uint<WDH_ph>		(&ph_i)		[DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2],
			const ap_uint<WDH_th11>		(&th11_i)	[DIM_th11_0]   [DIM_th11_1] [DIM_th11_2],
			const ap_uint<WDH_th>		(&th_i)		[DIM_th_0]     [DIM_th_1]   [DIM_th_2],
			const ap_uint<WDH_phzvl>	(&phzvl_i)	[DIM_phzvl_0]  [DIM_phzvl_1],
			const ap_uint<WDH_ph_hito>	(&ph_hit_i)	[DIM_ph_hito_0][DIM_ph_hito_1],

			// Outputs
			ap_uint<WDH_time> 		(&time_o)	[COORD_max_drift][DIM_time_0]   [DIM_time_1] [DIM_time_2],
			ap_uint<WDH_vl> 		(&vl_o)  	[COORD_max_drift][DIM_vl_0]     [DIM_vl_1],
			ap_uint<WDH_q>			(&q_o)		[COORD_max_drift][DIM_q_0]      [DIM_q_1]    [DIM_q_2],
			ap_uint<WDH_cpatr>		(&cpatr_o)	[COORD_max_drift][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2],
			ap_uint<WDH_ph>			(&ph_o)		[COORD_max_drift][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2],
			ap_uint<WDH_th11>		(&th11_o)	[COORD_max_drift][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2],
			ap_uint<WDH_th>			(&th_o)		[COORD_max_drift][DIM_th_0]     [DIM_th_1]   [DIM_th_2],
			ap_uint<WDH_phzvl>		(&phzvl_o)	[COORD_max_drift][DIM_phzvl_0]  [DIM_phzvl_1],
			ap_uint<WDH_ph_hito>	(&ph_hit_o)	[COORD_max_drift][DIM_ph_hito_0][DIM_ph_hito_1]
		#endif // __FULL_OUT__
	);

	// For legacy tb support
	#ifndef __FULL_OUT__
		ap_uint<WDH_time> 		time_o		[COORD_max_drift][DIM_time_0]   [DIM_time_1] [DIM_time_2];
		ap_uint<WDH_vl> 		vl_o  		[COORD_max_drift][DIM_vl_0]     [DIM_vl_1];
		ap_uint<WDH_q>			q_o			[COORD_max_drift][DIM_q_0]      [DIM_q_1]    [DIM_q_2];
		ap_uint<WDH_cpatr>		cpatr_o		[COORD_max_drift][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2];
		ap_uint<WDH_ph>			ph_o		[COORD_max_drift][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2];
		ap_uint<WDH_th11>		th11_o		[COORD_max_drift][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2];
		ap_uint<WDH_th>			th_o		[COORD_max_drift][DIM_th_0]     [DIM_th_1]   [DIM_th_2];
		ap_uint<WDH_phzvl>		phzvl_o		[COORD_max_drift][DIM_phzvl_0]  [DIM_phzvl_1];
		ap_uint<WDH_ph_hito>	ph_hit_o	[COORD_max_drift][DIM_ph_hito_0][DIM_ph_hito_1];
	#endif // __FULL_OUT__

};

template<typename GEN, int bx, int delay,
         int dim_j, int dim_k>
void coord_delay_array2_d(
	const GEN (&array_i)[dim_j][dim_k],
	GEN (&t_array_i_1)[delay][dim_j][dim_k],
	GEN (&array_o)[bx][dim_j][dim_k]
);


template<typename GEN, int bx, int delay,
         int dim_j, int dim_k, int dim_l>
void coord_delay_array3_d(
	GEN const (&array_i)[dim_j][dim_k][dim_l],
	GEN (&t_array_i_1)[delay][dim_j][dim_k][dim_l],
	GEN (&array_o)[bx][dim_j][dim_k][dim_l]
);


#endif // _COORD_DELAY_
