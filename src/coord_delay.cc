#include "../interface/coord_delay.h"
using namespace std;


struct coord_delay_struct{
	//ap_uint<1> pr;

	/*** FIRST BUNCH CROSSING *****/
	ap_uint<WDH_time> 		t_time_i_1		[DELAY_0][DIM_time_0]   [DIM_time_1] [DIM_time_2];
	ap_uint<WDH_vl> 		t_vl_i_1  		[DELAY_1][DIM_vl_0]     [DIM_vl_1];
	ap_uint<WDH_q>			t_q_i_1			[DELAY_0][DIM_q_0]      [DIM_q_1]    [DIM_q_2];
	ap_uint<WDH_cpatr>		t_cpatr_i_1		[DELAY_1][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2];
	ap_uint<WDH_ph>			t_ph_i_1		[DELAY_1][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2];
	ap_uint<WDH_th11>		t_th11_i_1		[DELAY_1][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2];
	ap_uint<WDH_th>			t_th_i_1		[DELAY_1][DIM_th_0]     [DIM_th_1]   [DIM_th_2];
	ap_uint<WDH_phzvl>		t_phzvl_i_1		[DELAY_1][DIM_phzvl_0]  [DIM_phzvl_1];
	ap_uint<WDH_ph_hito>	t_ph_hit_i_1	[DELAY_1][DIM_ph_hito_0][DIM_ph_hito_1];


	void coord_delay_actual(
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
		ap_uint<WDH_time>	 	(&time_o)	[COORD_max_drift][DIM_time_0]   [DIM_time_1] [DIM_time_2],
		ap_uint<WDH_vl> 		(&vl_o)  	[COORD_max_drift][DIM_vl_0]     [DIM_vl_1],
		ap_uint<WDH_q>			(&q_o)		[COORD_max_drift][DIM_q_0]      [DIM_q_1]    [DIM_q_2],
		ap_uint<WDH_cpatr>		(&cpatr_o)	[COORD_max_drift][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2],
		ap_uint<WDH_ph>			(&ph_o)		[COORD_max_drift][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2],
		ap_uint<WDH_th11>		(&th11_o)	[COORD_max_drift][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2],
		ap_uint<WDH_th>			(&th_o)		[COORD_max_drift][DIM_th_0]     [DIM_th_1]   [DIM_th_2],
		ap_uint<WDH_phzvl>		(&phzvl_o)	[COORD_max_drift][DIM_phzvl_0]  [DIM_phzvl_1],
		ap_uint<WDH_ph_hito>	(&ph_hit_o)	[COORD_max_drift][DIM_ph_hito_0][DIM_ph_hito_1]
	);
};

void coord_delay::operator()	(
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

	// Normal IO
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
){
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE ap_ctrl_none port=return

	static coord_delay_struct inst;
	//inst.pr=0;
	inst.coord_delay_actual(
		time_i, vl_i, q_i, cpatr_i, ph_i, 
		th11_i, th_i, phzvl_i, ph_hit_i,

		time_o, vl_o, q_o, cpatr_o, ph_o,
		th11_o, th_o, phzvl_o, ph_hit_o
	);
}


void coord_delay_struct::coord_delay_actual(
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
){
#pragma HLS PROTOCOL floating
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1


	/*** FIRST BUNCH CROSSING *****/
	/* // Unecessary???
	volatile ap_uint<WDH_time> 		a_time_i_1		[DELAY_0][DIM_time_0]   [DIM_time_1] [DIM_time_2];
#pragma HLS ARRAY_PARTITION variable=a_time_i_1 complete dim=0
	volatile ap_uint<WDH_vl> 		a_vl_i_1  		[DELAY_1][DIM_vl_0]     [DIM_vl_1];
#pragma HLS ARRAY_PARTITION variable=a_vl_i_1 complete dim=0
	volatile ap_uint<WDH_q>			a_q_i_1			[DELAY_0][DIM_q_0]      [DIM_q_1]    [DIM_q_2];
#pragma HLS ARRAY_PARTITION variable=a_q_i_1 complete dim=0
	volatile ap_uint<WDH_cpatr>		a_cpatr_i_1		[DELAY_1][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2];
#pragma HLS ARRAY_PARTITION variable=a_cpatr_i_1 complete dim=0
	volatile ap_uint<WDH_ph>		a_ph_i_1		[DELAY_1][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2];
#pragma HLS ARRAY_PARTITION variable=a_ph_i_1 complete dim=0
	volatile ap_uint<WDH_th11>		a_th11_i_1		[DELAY_1][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2];
#pragma HLS ARRAY_PARTITION variable=a_th11_i_1 complete dim=0
	volatile ap_uint<WDH_th>		a_th_i_1		[DELAY_1][DIM_th_0]     [DIM_th_1]   [DIM_th_2];
#pragma HLS ARRAY_PARTITION variable=a_th_i_1 complete dim=0
	volatile ap_uint<WDH_phzvl>		a_phzvl_i_1		[DELAY_1][DIM_phzvl_0]  [DIM_phzvl_1];
#pragma HLS ARRAY_PARTITION variable=a_phzvl_i_1 complete dim=0
	volatile ap_uint<WDH_ph_hito>	a_ph_hit_i_1	[DELAY_1][DIM_ph_hito_0][DIM_ph_hito_1];
#pragma HLS ARRAY_PARTITION variable=a_ph_hit_i_1 complete dim=0
	*/
	
	ap_uint<WDH_time> 		t_time_i		[DELAY_0][DIM_time_0]   [DIM_time_1] [DIM_time_2];
	ap_uint<WDH_vl> 		t_vl_i  		[DELAY_1][DIM_vl_0]     [DIM_vl_1];
	ap_uint<WDH_q>			t_q_i			[DELAY_0][DIM_q_0]      [DIM_q_1]    [DIM_q_2];
	ap_uint<WDH_cpatr>		t_cpatr_i		[DELAY_1][DIM_cpatr_0]  [DIM_cpatr_1][DIM_cpatr_2];
	ap_uint<WDH_ph>			t_ph_i			[DELAY_1][DIM_ph_0]     [DIM_ph_1]   [DIM_ph_2];
	ap_uint<WDH_th11>		t_th11_i		[DELAY_1][DIM_th11_0]   [DIM_th11_1] [DIM_th11_2];
	ap_uint<WDH_th>			t_th_i			[DELAY_1][DIM_th_0]     [DIM_th_1]   [DIM_th_2];
	ap_uint<WDH_phzvl>		t_phzvl_i		[DELAY_1][DIM_phzvl_0]  [DIM_phzvl_1];
	ap_uint<WDH_ph_hito>	t_ph_hit_i		[DELAY_1][DIM_ph_hito_0][DIM_ph_hito_1];
	
	// Delay Logic
	coord_delay_array3_d(time_i,	t_time_i_1,		time_o);
	coord_delay_array2_d(vl_i,		t_vl_i_1,		vl_o);
	coord_delay_array3_d(q_i,		t_q_i_1,		q_o);
	coord_delay_array3_d(cpatr_i,	t_cpatr_i_1,	cpatr_o);
	coord_delay_array3_d(ph_i,		t_ph_i_1,		ph_o);
	coord_delay_array3_d(th11_i,	t_th11_i_1,		th11_o);
	coord_delay_array3_d(th_i,		t_th_i_1,		th_o);
	coord_delay_array2_d(phzvl_i,	t_phzvl_i_1,	phzvl_o);
	coord_delay_array2_d(ph_hit_i,	t_ph_hit_i_1,	ph_hit_o);
}

template<typename GEN, int bx, int delay,
         int dim_j, int dim_k>
void coord_delay_array2_d(
	const GEN (&array_i)[dim_j][dim_k],
	GEN (&t_array_i_1)[delay][dim_j][dim_k],
	GEN (&array_o)[bx][dim_j][dim_k]
){
#pragma HLS INLINE
#if defined(__ENABLE_DLY_BYPASS__) && !defined(__SYNTHESIS__)
	for (int j = 0; j < dim_j; j++){
		for (int k = 0; k < dim_k; k++){
			array_o[0][j][k] = array_i[j][k];
		}
	}
#else
	// reg wire
	GEN	t_array_i[delay][dim_j][dim_j];

	// reg wire = reg contents
	for (int dly = 0; dly < (delay - 2); dly++){
	#pragma HLS unroll
		for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				t_array_i[dly][j][k] = t_array_i_1[dly][j][k];
			}
		}
	}

	// Input
	for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
		for (int k = 0; k < dim_k; k++){
			t_array_i[delay-2][j][k] = array_i[j][k];
		}
	}

	// Shift Reg
	for (int dly = 0; dly < (delay - 2); dly++){
	#pragma HLS unroll
		for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				t_array_i_1[dly][j][k] = t_array_i[dly+1][j][k];
			}
		}
	}

	// Output
	//#if COORD_max_drift == 1 // (for max_drift = 1)
	for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
		for (int k = 0; k < dim_k; k++){
			array_o[0][j][k] = t_array_i[0][j][k];
		}
	}
	/*#elif COORD_max_drift == 3 // (for max_drift = 3)
		for (int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				ph_hit_o[0][j][k] = t_array_i[2][j][k];
			}
		}
		for (int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				ph_hit_o[1][j][k] = t_array_i[1][j][k];
			}
		}
		for (int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				ph_hit_o[2][j][k] = t_array_i[0][j][k];
			}
		}
	#endif*/
#endif
}


template<typename GEN, int bx, int delay,
         int dim_j, int dim_k, int dim_l>
void coord_delay_array3_d(
	GEN const (&array_i)[dim_j][dim_k][dim_l],
	GEN (&t_array_i_1)[delay][dim_j][dim_k][dim_l],
	GEN (&array_o)[bx][dim_j][dim_k][dim_l]
){
#pragma HLS INLINE
#if defined(__ENABLE_DLY_BYPASS__) && !defined(__SYNTHESIS__)
	for (int j = 0; j < dim_j; j++){
		for (int k = 0; k < dim_k; k++){
			for (int l = 0; l < dim_l; l++){
				array_o[0][j][k][l] = array_i[j][k][l];
			}
		}
	}
#else
	GEN t_array_i[delay][dim_j][dim_k][dim_l];

	for (int dly = 0; dly < (delay - 2); dly++){
	#pragma HLS unroll
		for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				for (int l = 0; l < dim_l; l++){
					t_array_i[dly][j][k][l] = t_array_i_1[dly][j][k][l];
				}
			}
		}
	}

	// Input
	for (int j = 0; j < dim_j; j++){
	#pragma HLS unroll
		for (int k = 0; k < dim_k; k++) {
			for (int l = 0; l < dim_l; l++) {
				t_array_i[delay-2][j][k][l] = array_i[j][k][l];
			}
		}
	}

	// Shift Reg
	for(int dly = 0; dly < (delay - 2); dly++){
	#pragma HLS unroll
		for(int j = 0; j < dim_j; j++){
	#pragma HLS UNROLL
			for(int k = 0; k < dim_k; k++){
				for(int l = 0; l < dim_l; l++){
					//a_th_i_1[dly][i][j][k]=t_th_i[dly][i][j][k];
					t_array_i_1[dly][j][k][l]=t_array_i[dly+1][j][k][l];
				}
			}
		}
	}

	// Output
	//#if COORD_max_drift == 1 // (for max_drift = 1)
		for(int j = 0; j < dim_j; j++){
	#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				for (int l = 0; l < dim_l; l++){
					array_o[0][j][k][l] = t_array_i[0][j][k][l];
				}
			}
		}
	/*#elif COORD_max_drift == 3 // (for max_drift = 3)
		for(int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				for (int l = 0; l < dim_l; l++){
					array_o[2][j][k][l] = t_array_i[0][j][k][l];
				}
			}
		}
		for(int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				for (int l = 0; l < dim_l; l++){
					array_o[1][j][k][l] = t_array_i[1][j][k][l];
				}
			}
		}
		for(int j = 0; j < dim_j; j++){
		#pragma HLS unroll
			for (int k = 0; k < dim_k; k++){
				for (int l = 0; l < dim_l; l++){
					array_o[0][j][k][l] = t_array_i[2][j][k][l];
				}
			}
		}
	#endif*/
#endif
}