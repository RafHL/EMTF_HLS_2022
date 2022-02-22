// Includes
#include "io.h"
#include "parsed_io.h"
#include "wrapper.h"
#include <iterator>
#include <sstream>
#include <fstream>
#include <optional>


// Function Declarations
template<typename GEN>
std::vector<GEN> move_to_new_vec(
        std::vector<GEN> &vec1, std::vector<GEN> &vec2
);


int main(int argc, char* argv[]) {
    // Argument handling
    std::string parsed_filepath;
    std::string template_filepath;
    std::string wrapper_filepath;
    if (argc == 4) {
        parsed_filepath = argv[1];
        template_filepath = argv[2];
        wrapper_filepath = argv[3];
    } else if (argc == 1) {
        parsed_filepath = default_parsed_filepath;
        template_filepath = default_template_filepath;
        wrapper_filepath = default_wrapper_filepath;
    } else {
        std::cerr << "\nERROR: Main: Invalid number of arguments\n\n";
        return -1;
    }


    // Usefull Lambdas
    auto lmbd_adv_all = [](int x, auto& ... iter){ (std::advance(iter, x), ...); };
    auto lmbd_make_variadic = [&](auto& func, auto& ... args){ (func(args), ...); };

    
    // Get parsed IO
    std::array<parsed_IO::parsed_data::parsed_data_t, 2> data;
    data = parsed_IO::parsed_data::from_file(parsed_filepath);
    auto parsed_IO_in = parsed_IO::parsed_data::to_parsed_IO(data[0], parsed_IO::INPUT_REG);
    auto parsed_IO_out = parsed_IO::parsed_data::to_parsed_IO(data[1], parsed_IO::OUTPUT_REG);
    auto parsed_IO = move_to_new_vec(parsed_IO_in, parsed_IO_out);

    
    // Create necessary components for output generation
    /// Components for output generation
    std::vector<std::string> module_io_clk_rst;
    std::vector<std::string> module_io_busses;
    std::vector<std::string> module_io_ctrl_wires;
    std::vector<std::string> wrapper_reg_decl; /// Create register instantiations for the wrapper
    std::vector<std::string> wrapper_wire_decl; /// Create Connection Wires declarations
    std::vector<std::string> tl_conn_clk_rst;
    std::vector<std::vector<std::string>> sub_tl_conn_unrolled_wires; /// subcomponent - Creating names of inputs and outputs of top level file (i.e. "phi_0_0_0_V"..."reg_phi_5_8_1_V")
    std::vector<std::vector<std::string>> sub_tl_conn_unrolled_regs; /// subcomponent - Create explicit register array names (i.e. "reg_phi[0][0][0]"..."reg_phi[5][8][1]")
    std::vector<std::vector<std::string>> tl_connections; /// Create connection wires
    std::vector<std::vector<std::string>> tl_conn_ap_vld_unconn;
    std::vector<std::string> shift_in_logic; /// Create shift in logic
    std::vector<std::string> shift_out_logic; /// Create shift out logic
    std::vector<std::string> reset_logic; /// Creates the reset logic for IO registers + output busses

    /// Handle the clock and reset
    //// Check that ap_clk and ap_rst exist
    {
        bool has_clk = false;
        bool has_rst = false;
        for (const auto& io : parsed_IO) {
            if ( io.io_name == "ap_clk" ) { has_clk = true; }
            if ( io.io_name == "ap_rst" ) { has_rst = true; }
        }
        if ( ( has_clk != true ) || ( has_rst != true ) ) {
            std::cerr << "\nERROR: Main: Missing \"ap_clk\" and/or \"ap_rst\" as io from parsed io\n\n";
            return -1;
        }
    }

    //// Create wrapper module IO instantiaions for clock and reset
    module_io_clk_rst.push_back("input wire clock,\n");
    module_io_clk_rst.push_back("input wire reset,\n");

    //// Create wrapper connections for wrapper to tl for clock and reset
    tl_conn_clk_rst.push_back(".ap_clk(clock),\n");
    tl_conn_clk_rst.push_back(".ap_rst(reset),\n");

    /// Create wrapper module io for bus wires/registers
    for (const auto& io : parsed_IO) {
        if ((io.io_name != "ap_clk") && (io.io_name != "ap_rst")) {
            module_io_busses.push_back( wrapper_gen::module_io::make_buses(io.io_name, io.io_type, io.io_bus) );
        }
    }
    
    /// Create wrapper module io for control wires
    for (const auto& io : parsed_IO) {
        if ((io.io_name != "ap_clk") && (io.io_name != "ap_rst")) {
            module_io_ctrl_wires.push_back( wrapper_gen::module_io::make_ctrl_wires(io.io_name, io.io_type) );
        }
    }
    module_io_ctrl_wires.back().pop_back();
    module_io_ctrl_wires.back().pop_back();
    module_io_ctrl_wires.back().push_back('\n');

    /// creates many of the (sub)components that soley rely on parsed_IO and not other (sub)components
    for (const auto& io : parsed_IO) {
        if ((io.io_name != "ap_clk") && (io.io_name != "ap_rst")) {
            wrapper_reg_decl.push_back(
                wrapper_gen::reg_wire_decl::make_rolled_regs(io.io_name, io.io_bus, io.io_dim)
            );
            sub_tl_conn_unrolled_wires.push_back(
                wrapper_gen::tl_conn::make_tl_names(io.io_name, io.io_dim)
            );
            sub_tl_conn_unrolled_regs.push_back(
                wrapper_gen::tl_conn::make_unrolled_regs(io.io_name, io.io_dim)
            );
            tl_connections.push_back(
                wrapper_gen::tl_conn::make_unrolled_wires(io.io_name, io.io_dim)
            );
            reset_logic.push_back(
                wrapper_gen::reset_logic::make_reg_reset(io.io_name, io.io_dim, io.io_type)
            );

            if (io.io_type == parsed_IO::INPUT_REG) {
                shift_in_logic.push_back(
                    wrapper_gen::shift_logic::make_shift_in(io.io_name, io.io_dim)
                );
            } else if (io.io_type == parsed_IO::OUTPUT_REG) {
                wrapper_wire_decl.push_back(
                    wrapper_gen::reg_wire_decl::make_rolled_wires(io.io_name, io.io_bus, io.io_dim)
                );
                shift_out_logic.push_back(
                    wrapper_gen::shift_logic::make_shift_out(io.io_name, io.io_dim)
                );
            }   
        }
    }

    /// connect top level io to their corresponding unrolled wrapper registers/wires (i.e. ".pho_0_0_0_1_V(wire_pho[0][0][0][1]),")
    std::vector<std::vector<std::string>> tl_io_connected;
    {
        auto itr0 = parsed_IO.begin();
        auto itr1 = sub_tl_conn_unrolled_wires.begin();
        auto itr2 = sub_tl_conn_unrolled_regs.begin();
        auto itr3 = tl_connections.begin();
        for (; itr0 != parsed_IO.end(); std::advance(itr0, 1)){
            if (((*itr0).io_name != "ap_clk") && ((*itr0).io_name != "ap_rst")) {
                std::optional<std::vector<std::string>> opt_temp_ret;
                std::vector<std::string> temp_ret;

                // Inputs (direct connection to output of registers)
                if ((*itr0).io_type == parsed_IO::reg_io_type::INPUT_REG) {
                    opt_temp_ret = wrapper_gen::tl_conn::make_wrapper_tl_conn(*itr1, *itr2, (*itr0).io_dim);
                }
                // Outputs (connect to a intermediary wire before connecting to register (prevents multiple drivers))
                else {
                    opt_temp_ret = wrapper_gen::tl_conn::make_wrapper_tl_conn(*itr1, *itr3, (*itr0).io_dim);
                }

                try {
                    temp_ret = opt_temp_ret.value();
                } catch (const std::bad_optional_access& e) {
                    std::cerr << "\nERROR: Main: function \"make_wrapper_tl_conn\" returned nothing, it encounted an error!\n\n";
                    return -1;
                }
                tl_io_connected.push_back(temp_ret);

                lmbd_adv_all(1, itr1, itr2, itr3);
            }
        }
    }

    /// Create all the _ap_vld, and leave them unconnected
    {
        auto itr0 = parsed_IO.begin();
        auto itr1 = sub_tl_conn_unrolled_wires.begin();
        for (; (((itr0 != parsed_IO.end()) && (itr1 != sub_tl_conn_unrolled_wires.end()))); 
                std::advance(itr0, 1)
        ){
            if (((*itr0).io_name != "ap_clk") && ((*itr0).io_name != "ap_rst")) {
                // Output & has dimensions, create "_ap_vld" and connect it to nothing
                if (((*itr0).io_type == parsed_IO::OUTPUT_REG) && !((*itr0).io_dim.empty())){
                    tl_conn_ap_vld_unconn.push_back(std::vector<std::string>());
                    for (auto itr1_sub = (*itr1).begin(); itr1_sub != (*itr1).end(); std::advance(itr1_sub, 1)) {
                        std::stringstream ss_entry;
                        ss_entry << "." << *itr1_sub << "_ap_vld(),\n";
                        tl_conn_ap_vld_unconn.back().push_back(ss_entry.str());
                    }
                }
                std::advance(itr1, 1);
            }
        }
        // remove the comma at the end of the last connection entry
        tl_conn_ap_vld_unconn.back().back().pop_back();
        tl_conn_ap_vld_unconn.back().back().pop_back();
        tl_conn_ap_vld_unconn.back().back().push_back('\n');
    }


    // load template
    std::optional<std::vector<std::string>> opt_template = w_template::from_file(template_filepath);
    std::vector<std::string> template_vec;
    try {
        template_vec = opt_template.value();
    } catch (const std::bad_optional_access& e) {
        std::cerr << "\nERROR: Main: No Template Loaded\n\n";
        return -1;
    }


    // output to file
    /// Stringstream for storing output + lambdas for constructing the output
    std::stringstream ss_output;
    auto lmdb_vec1_to_ss_output = [&ss_output](std::vector<auto> &dump_vec1){
        for (auto itr = dump_vec1.begin(); itr < dump_vec1.end(); std::advance(itr, 1)){
            ss_output << *itr;
        }
    };
    auto lmbd_vec2_to_ss_output = [&ss_output](std::vector<auto> &dump_vec2){
        for (auto itr = dump_vec2.begin(); itr != dump_vec2.end(); std::advance(itr, 1)) {
            for (auto itr_sub = (*itr).begin(); itr_sub != (*itr).end(); std::advance(itr_sub, 1)) {
                ss_output << *itr_sub;
            }
        }
    };
    auto lmbd_shift_to_ss_output = [&ss_output](
        std::vector<auto> &check_vec,
        std::vector<auto> &shift_i_vec,
        std::vector<auto> &shift_o_vec
    ){
        auto itr0 = check_vec.begin();
        auto itr1 = shift_i_vec.begin();
        auto itr2 = shift_o_vec.begin();
        for (; itr0 != check_vec.end(); std::advance(itr0, 1)) {
            if (((*itr0).io_name != "ap_clk") && ((*itr0).io_name != "ap_rst")) {
                if ((*itr0).io_type == parsed_IO::INPUT_REG) {
                    ss_output << "\n// SHIFT LOGIC - INPUT: [" << (*itr0).io_name << "]\n\n";
                    ss_output << *itr1;
                    std::advance(itr1, 1);
                } else if ((*itr0).io_type == parsed_IO::OUTPUT_REG) {
                    ss_output << "\n// SHIFT LOGIC - OUTPUT: [" << (*itr0).io_name << "]\n\n";
                    ss_output << *itr2;
                    std::advance(itr2, 1);
                }
            }
        }
    };

    /// Construct output
    for (const auto& io : template_vec) {
        if        (io == "//<<module_io>>") {
            ss_output << "\n// Module IO - Clock/Reset\n\n";
            lmdb_vec1_to_ss_output(module_io_clk_rst);

            ss_output << "\n// Module IO - Bus Signals\n\n";
            lmdb_vec1_to_ss_output(module_io_busses);

            ss_output << "\n// Module IO - Control Signals\n\n";
            lmdb_vec1_to_ss_output(module_io_ctrl_wires);

        } else if (io == "//<<reg_wire_dec>>") {
            ss_output << "\n// Register/Wire Declarations\n\n";
            lmbd_make_variadic(lmdb_vec1_to_ss_output, wrapper_reg_decl, wrapper_wire_decl);

        } else if (io == "//<<tl_conn>>") {
            ss_output << "\n// IO Connections\n\n";
            lmbd_make_variadic(lmbd_vec2_to_ss_output, tl_conn_clk_rst, tl_io_connected, tl_conn_ap_vld_unconn);
            
        } else if (io == "//<<rst_logic>>") {
            ss_output << "\n// RESET LOGIC\n\n";
            lmdb_vec1_to_ss_output(reset_logic);
            
        } else if (io == "//<<shift_logic>>") {
            ss_output << "\n// SHIFT LOGIC\n\n";
            lmbd_shift_to_ss_output(parsed_IO, shift_in_logic, shift_out_logic);
            
        } else {
            ss_output << io << "\n";
        }
    }

    /// Output the ss_output to the output file
    std::ofstream output_file(wrapper_filepath, std::ios::trunc);
    if (output_file.fail()) { std::cout << "Failed to open file\n"; return -1; }
    output_file << ss_output.rdbuf();

    return 0;
}


// Function Definitions
/// Moves the elements of two vectors to a new vector
///     <<side-effect>>: erases the input vectors
template<typename GEN>
std::vector<GEN> move_to_new_vec(
    std::vector<GEN> &vec1, std::vector<GEN> &vec2
){
    // Instantiate new vector
    std::vector<GEN> ret_vec = std::vector<GEN>();

    // Move vec1 to beginning of ret_vec, and move vec2 to after where vec1 has been moved
    ret_vec.insert(ret_vec.begin(), std::make_move_iterator(vec1.begin()), std::make_move_iterator(vec1.end()));
    ret_vec.insert(ret_vec.end(), std::make_move_iterator(vec2.begin()), std::make_move_iterator(vec2.end()));

    // Erase vec1 & vec2 (their elements have been moved!)
    vec1.erase(vec1.begin(), vec1.end());
    vec2.erase(vec2.begin(), vec2.end());

    return ret_vec;
}