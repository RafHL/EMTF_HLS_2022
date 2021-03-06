#ifndef __EMTF_HLSLIB_FULLYCONNECT_H__
#define __EMTF_HLSLIB_FULLYCONNECT_H__

// Function hierarchy
//
// fullyconnect_layer
// +-- fullyconnect_op (INLINE)
//     |-- fullyconnect_preprocessing_op
//     |-- fullyconnect_dense_op
//     |-- fullyconnect_activation_op
//     |-- fullyconnect_dense_op
//     |-- fullyconnect_activation_op
//     |-- fullyconnect_dense_op
//     |-- fullyconnect_activation_op
//     +-- fullyconnect_dense_final_op

// EMTF HLS
#include "layer_helpers.h"
#include "nnet_kernels.h"

namespace emtf {

namespace phase2 {

template <typename Category, typename T_IN, typename T_OUT>
void fullyconnect_activation_op(const T_IN in0[detail::nnet_num_outbound_nodes_traits<Category>::value],
                                T_OUT out[detail::nnet_num_outbound_nodes_traits<Category>::value]) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
  //#pragma HLS INLINE
  // hls-pragmas end

  const unsigned int N = detail::nnet_num_outbound_nodes_traits<Category>::value;

  detail::vector_tanh_activate_op<N>(in0, out);
}

template <typename Category, typename T_IN, typename T_OUT>
void fullyconnect_preprocessing_op(const T_IN in0[detail::nnet_num_inbound_nodes_traits<Category>::value],
                                   T_OUT out[detail::nnet_num_outbound_nodes_traits<Category>::value]) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
  //#pragma HLS INLINE
  // hls-pragmas end

  typedef typename detail::select_nnet_weight_type<Category>::type weight_t;
  const unsigned int M = detail::nnet_num_inbound_nodes_traits<Category>::value;
  const unsigned int N = detail::nnet_num_outbound_nodes_traits<Category>::value;

#ifndef __SYNTHESIS__
  static bool initialized = false;
  static weight_t weights[N];
#else
  bool initialized = false;
  weight_t weights[N];
#endif  // __SYNTHESIS__ not defined

  if (!initialized) {
    initialized = true;
    detail::init_nnet_weights_op<N>(weights, detail::get_nnet_weights_op<Category>());
  }

  ap_fixed<T_IN::width, T_IN::width> in0_cast[N];  // cast from ap_int to ap_fixed

  // hls-pragmas begin
#pragma HLS ARRAY_PARTITION variable = weights complete dim = 0
#pragma HLS ARRAY_PARTITION variable = in0_cast complete dim = 0
  // hls-pragmas end

  emtf_assert(M == N);
  detail::vector_cast_op<N>(in0, in0_cast);
  detail::vec_vec_mult_op<N>(in0_cast, weights, out);
}

template <typename Category, typename T_IN, typename T_OUT>
void fullyconnect_dense_op(const T_IN in0[detail::nnet_num_inbound_nodes_traits<Category>::value],
                           T_OUT out[detail::nnet_num_outbound_nodes_traits<Category>::value]) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
  //#pragma HLS INLINE
  // hls-pragmas end

  typedef typename detail::select_nnet_weight_type<Category>::type weight_t;
  typedef typename detail::select_nnet_weight_type<Category>::type bias_t;
  const unsigned int M = detail::nnet_num_inbound_nodes_traits<Category>::value;
  const unsigned int N = detail::nnet_num_outbound_nodes_traits<Category>::value;

#ifndef __SYNTHESIS__
  static bool initialized = false;
  static weight_t weights[M * N];
  static bias_t biases[N];
#else
  bool initialized = false;
  weight_t weights[M * N];
  bias_t biases[N];
#endif  // __SYNTHESIS__ not defined

  if (!initialized) {
    initialized = true;
    detail::init_nnet_weights_op<M * N>(weights, detail::get_nnet_weights_op<Category>());
    detail::init_nnet_weights_op<N>(biases, detail::get_nnet_biases_op<Category>());
  }

  // hls-pragmas begin
#pragma HLS ARRAY_PARTITION variable = weights complete dim = 0
#pragma HLS ARRAY_PARTITION variable = biases complete dim = 0
  // hls-pragmas end

  detail::mat_vec_mult_biasadd_op<M, N>(in0, weights, biases, out);
}

template <typename Category, typename T_IN, typename T_OUT>
void fullyconnect_dense_final_op(const T_IN in0[detail::nnet_num_inbound_nodes_traits<Category>::value],
                                 T_OUT out[detail::nnet_num_outbound_nodes_traits<Category>::value]) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
  //#pragma HLS INLINE
  // hls-pragmas end

  typedef typename detail::select_nnet_weight_type<Category>::type weight_t;
  typedef typename detail::select_nnet_weight_type<Category>::type bias_t;
  const unsigned int M = detail::nnet_num_inbound_nodes_traits<Category>::value;
  const unsigned int N = detail::nnet_num_outbound_nodes_traits<Category>::value;

#ifndef __SYNTHESIS__
  static bool initialized = false;
  static weight_t weights[M * N];
  static bias_t biases[N];
#else
  bool initialized = false;
  weight_t weights[M * N];
  bias_t biases[N];
#endif  // __SYNTHESIS__ not defined

  if (!initialized) {
    initialized = true;
    detail::init_nnet_weights_op<M * N>(weights, detail::get_nnet_weights_op<Category>());
    detail::init_nnet_weights_op<N>(biases, detail::get_nnet_biases_op<Category>());
  }

  // hls-pragmas begin
#pragma HLS ARRAY_PARTITION variable = weights complete dim = 0
#pragma HLS ARRAY_PARTITION variable = biases complete dim = 0
  // hls-pragmas end

  emtf_assert(N == 1);
  detail::vec_vec_mult_biasadd_op<M>(in0, weights, biases[0], out[0]);
}

// _____________________________________________________________________________
// Fully connected op

template <typename Zone>
void fullyconnect_op(const trk_feat_t curr_trk_feat_rm[num_emtf_features], trk_invpt_t& curr_trk_invpt,
                     trk_phi_t& curr_trk_phi, trk_eta_t& curr_trk_eta, trk_d0_t& curr_trk_d0, trk_z0_t& curr_trk_z0,
                     trk_beta_t& curr_trk_beta) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INLINE
  // hls-pragmas end

  const unsigned int n_layer_0 = detail::nnet_num_outbound_nodes_traits<m_nnet_0_layer_0_tag>::value;
  const unsigned int n_layer_1 = detail::nnet_num_outbound_nodes_traits<m_nnet_0_layer_1_tag>::value;
  const unsigned int n_layer_2 = detail::nnet_num_outbound_nodes_traits<m_nnet_0_layer_2_tag>::value;
  const unsigned int n_layer_3 = detail::nnet_num_outbound_nodes_traits<m_nnet_0_layer_3_tag>::value;
  const unsigned int n_layer_4 = detail::nnet_num_outbound_nodes_traits<m_nnet_0_layer_4_tag>::value;

  // Preactivation types, only needed for the hidden dense layers
  typedef detail::select_nnet_preactivation_type<m_nnet_0_layer_1_tag>::type layer_1_preact_t;
  typedef detail::select_nnet_preactivation_type<m_nnet_0_layer_2_tag>::type layer_2_preact_t;
  typedef detail::select_nnet_preactivation_type<m_nnet_0_layer_3_tag>::type layer_3_preact_t;

  // Output types
  typedef detail::select_nnet_activation_type<m_nnet_0_layer_0_tag>::type layer_0_out_t;
  typedef detail::select_nnet_activation_type<m_nnet_0_layer_1_tag>::type layer_1_out_t;
  typedef detail::select_nnet_activation_type<m_nnet_0_layer_2_tag>::type layer_2_out_t;
  typedef detail::select_nnet_activation_type<m_nnet_0_layer_3_tag>::type layer_3_out_t;
  typedef detail::select_nnet_activation_type<m_nnet_0_layer_4_tag>::type layer_4_out_t;

  // Intermediate arrays
  layer_1_preact_t layer_1_preact[n_layer_1];
  layer_2_preact_t layer_2_preact[n_layer_2];
  layer_3_preact_t layer_3_preact[n_layer_3];
  layer_0_out_t layer_0_out[n_layer_0];
  layer_1_out_t layer_1_out[n_layer_1];
  layer_2_out_t layer_2_out[n_layer_2];
  layer_3_out_t layer_3_out[n_layer_3];
  layer_4_out_t layer_4_out[n_layer_4];

  // hls-pragmas begin
#pragma HLS ARRAY_PARTITION variable = layer_1_preact complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_2_preact complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_3_preact complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_0_out complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_1_out complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_2_out complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_3_out complete dim = 0
#pragma HLS ARRAY_PARTITION variable = layer_4_out complete dim = 0
  // hls-pragmas end

  // Layer 0 - preprocessing
  fullyconnect_preprocessing_op<m_nnet_0_layer_0_tag>(curr_trk_feat_rm, layer_0_out);

  // Layer 1 - dense + activation
  fullyconnect_dense_op<m_nnet_0_layer_1_tag>(layer_0_out, layer_1_preact);
  fullyconnect_activation_op<m_nnet_0_layer_1_tag>(layer_1_preact, layer_1_out);

  // Layer 2 - dense_1 + activation_1
  fullyconnect_dense_op<m_nnet_0_layer_2_tag>(layer_1_out, layer_2_preact);
  fullyconnect_activation_op<m_nnet_0_layer_2_tag>(layer_2_preact, layer_2_out);

  // Layer 3 - dense_2 + activation_2
  fullyconnect_dense_op<m_nnet_0_layer_3_tag>(layer_2_out, layer_3_preact);
  fullyconnect_activation_op<m_nnet_0_layer_3_tag>(layer_3_preact, layer_3_out);

  // Layer 4 - dense_final
  fullyconnect_dense_final_op<m_nnet_0_layer_4_tag>(layer_3_out, layer_4_out);

  // Output
  // Reinterpret ap_fixed as ap_int
  curr_trk_invpt.range() = layer_4_out[0].range();
  curr_trk_phi.range() = 0;
  curr_trk_eta.range() = 0;
  curr_trk_d0.range() = 0;
  curr_trk_z0.range() = 0;
  curr_trk_beta.range() = 0;

  // std::cout << "[DEBUG] layer 0: ";
  // for (unsigned i = 0; i < n_layer_0; i++) {
  //   std::cout << layer_0_out[i] * (1 << 10) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "[DEBUG] layer 1: ";
  // for (unsigned i = 0; i < n_layer_1; i++) {
  //   std::cout << layer_1_out[i] * (1 << 13) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "[DEBUG] layer 2: ";
  // for (unsigned i = 0; i < n_layer_2; i++) {
  //   std::cout << layer_2_out[i] * (1 << 13) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "[DEBUG] layer 3: ";
  // for (unsigned i = 0; i < n_layer_3; i++) {
  //   std::cout << layer_3_out[i] * (1 << 13) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "[DEBUG] layer 4: ";
  // for (unsigned i = 0; i < n_layer_4; i++) {
  //   std::cout << layer_4_out[i] * (1 << 13) << " ";
  // }
  // std::cout << std::endl;
}

// _____________________________________________________________________________
// Entry point

template <typename Zone>
void fullyconnect_layer(const trk_feat_t curr_trk_feat_rm[num_emtf_features], trk_invpt_t& curr_trk_invpt,
                        trk_phi_t& curr_trk_phi, trk_eta_t& curr_trk_eta, trk_d0_t& curr_trk_d0, trk_z0_t& curr_trk_z0,
                        trk_beta_t& curr_trk_beta) {
  // hls-pragmas begin
#pragma HLS PIPELINE II = fullyconnect_config::layer_target_ii
#pragma HLS INTERFACE ap_ctrl_none port = return
  // hls-pragmas end

  // Check assumptions
  static_assert(fullyconnect_config::n_in == num_emtf_tracks, "fullyconnect_config::n_in check failed");
  static_assert(fullyconnect_config::n_out == num_emtf_tracks, "fullyconnect_config::n_out check failed");
  static_assert(num_emtf_features == 40, "num_emtf_features must be 40");

  fullyconnect_op<Zone>(curr_trk_feat_rm, curr_trk_invpt, curr_trk_phi, curr_trk_eta, curr_trk_d0, curr_trk_z0,
                        curr_trk_beta);
}

}  // namespace phase2

}  // namespace emtf

#endif  // __EMTF_HLSLIB_FULLYCONNECT_H__ not defined
