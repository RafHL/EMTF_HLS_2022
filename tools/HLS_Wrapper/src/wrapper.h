#ifndef _WRAPPER_H_
#define _WRAPPER_H_

// Includes
#include "parsed_io.h"
#include <iostream>
#include <array>
#include <vector>
#include <sstream>
#include <optional>


// Constants
static const uint max_dim = 4;


// Functions
namespace wrapper_gen {
    // Functions responsible for creating the module io of the wrapper file
    namespace module_io {
        /// creates the wrapper module's bus wires/registers for the parsed io
        std::string make_buses(
            const std::string &name, const parsed_IO::reg_io_type &type, const std::string &bus
        ){
            std::stringstream temp_ss;
            if (type == parsed_IO::INPUT_REG ) {
                temp_ss << "input  wire ";
            } else {
                temp_ss << "output reg ";
            }
            temp_ss << bus << " bus_" << name << ",\n";
            std::string out_string = temp_ss.str();
            return out_string;
        }

        /// creates the wrapper module's control wires
        std::string make_ctrl_wires(
            const std::string &name, const parsed_IO::reg_io_type &type
        ){
            std::stringstream temp_ss;
            if ( type == parsed_IO::INPUT_REG ) {
                temp_ss << "input wire feed_in_" << name << ",\n";;
            } else {
                temp_ss << "input wire feed_out_" << name << ",\n";;
                temp_ss << "input wire load_out_" << name << ",\n";;
            }
            return temp_ss.str();
        }
    }

    // Functions responsible for creating the register and wire declarations of the wrapper file
    namespace reg_wire_decl {
        /// creates the rolled, wrapper registers
        std::string make_rolled_regs(
            const std::string &common_name, const std::string &bus, const std::vector<int> &dimensions
        ){
            int num_dim = dimensions.size();
            std::string out_string = "";

            // iterate through dimensions vector
            for (auto itr = dimensions.begin(); itr != dimensions.end(); itr++){
                std::stringstream temp_ss;
                temp_ss << "[0:" << (*itr)-1 << "]";
                out_string += temp_ss.str();
            }

            // add basic register instantiation + bus
            std::stringstream temp_ss;
            temp_ss << "reg " << bus << " reg_" << common_name;
            out_string.insert(0, temp_ss.str());


            out_string += ";\n";
            return out_string;
        }

        // creates the rolled, wrapper wires
        std::string make_rolled_wires(
            const std::string &common_name, const std::string &bus, const std::vector<int> &dimensions
        ){
            int num_dim = dimensions.size();
            std::string out_string = "";

            // iterate through dimensions vector
            for (auto itr = dimensions.begin(); itr != dimensions.end(); itr++){
                std::stringstream temp_ss;
                temp_ss << "[0:" << (*itr)-1 << "]";
                out_string += temp_ss.str();
            }

            // add basic register instantiation + bus
            std::stringstream temp_ss;
            temp_ss << "wire " << bus << " wire_" << common_name;
            out_string.insert(0, temp_ss.str());


            out_string += ";\n";
            return out_string;
        }
    }

    namespace tl_conn {
        /// creates the io of the top level
        std::vector<std::string> make_tl_names(
            const std::string &common_name, const std::vector<int> &dimensions
        ){
            int num_dim = dimensions.size();
            std::vector<std::string> vec_output = std::vector<std::string>();

            for (uint i = 0; i < num_dim; i++) {
                //std::cout << common_name << " - Dimension " << i << " seen as: [" << dimensions[i] << "]\n";
            }

            if (num_dim == 0) {
                //std::cout << common_name << " - Dimension - NONE\n";
                std::stringstream ss_entry;
                ss_entry << common_name << "_V";
                vec_output.push_back(ss_entry.str());
            }
            else if ( num_dim == 1 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    std::stringstream ss_entry;
                    ss_entry << common_name << "_" << i << "_V";
                    vec_output.push_back(ss_entry.str());
                }
            }
            else if ( num_dim == 2) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        std::stringstream ss_entry;
                        ss_entry << common_name << "_" << i << "_" << j << "_V";
                        vec_output.push_back(ss_entry.str());
                    }
                }
            }
            else if ( num_dim == 3) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            std::stringstream ss_entry;
                            ss_entry << common_name << "_" << i << "_" << j << "_" << k << "_V";
                            vec_output.push_back(ss_entry.str());
                        }
                    }
                }
            }
            else if ( num_dim == 4 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            for (uint l = 0; l < dimensions[3]; l++) {
                                std::stringstream ss_entry;
                                ss_entry << common_name << "_" << i << "_" << j << "_" << k << "_" << l << "_V";
                                vec_output.push_back(ss_entry.str());
                            }
                        }
                    }
                }
            }
            else {
                std::cerr << "\nERROR: wrapper_gen::tl_conn::make_tl_names: Program doesn't understand a number of dimensions outside 0-4\n\n";
            }

            return vec_output;
        }

        /// Creates the unrolled register names of wrapper registers (for connection to top level)
        std::vector<std::string> make_unrolled_regs(
                const std::string &common_name, const std::vector<int> &dimensions
        ){
            int num_dim = dimensions.size();
            std::vector<std::string> vec_output = std::vector<std::string>();

            for (uint i = 0; i < num_dim; i++) {
                //std::cout << "reg_" << common_name << " - Dimension " << i << " seen as: [" << dimensions[i] << "]\n";
            }

            if (num_dim == 0) {
                //std::cout << common_name << " - Dimensions - NONE\n";
                std::stringstream ss_entry;
                ss_entry << "reg_" << common_name;
                vec_output.push_back(ss_entry.str());
            }
            else if ( num_dim == 1 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    std::stringstream ss_entry;
                    ss_entry << "reg_" << common_name << "[" << i << "]";
                    vec_output.push_back(ss_entry.str());
                }
            }
            else if ( num_dim == 2) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        std::stringstream ss_entry;
                        ss_entry << "reg_" << common_name << "[" << i << "][" << j << "]";
                        vec_output.push_back(ss_entry.str());
                    }
                }
            }
            else if ( num_dim == 3) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            std::stringstream ss_entry;
                            ss_entry << "reg_" << common_name << "[" << i << "][" << j << "][" << k << "]";
                            vec_output.push_back(ss_entry.str());
                        }
                    }
                }
            }
            else if ( num_dim == 4 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            for (uint l = 0; l < dimensions[3]; l++) {
                                std::stringstream ss_entry;
                                ss_entry << "reg_" << common_name << "[" << i << "][" << j << "][" << k << "][" << l << "]" ;
                                vec_output.push_back(ss_entry.str());
                            }
                        }
                    }
                }
            }
            else {
                std::cerr << "\nERROR: wrapper_gen::tl_conn::make_unrolled_regs: Program doesn't understand a number of dimensions outside 0-4\n\n";
            }

            return vec_output;
        }

        // Creates the unrolled wire names of wrapper wires (for connection to the top level)
        std::vector<std::string> make_unrolled_wires(
            const std::string &common_name, const std::vector<int> &dimensions
        ){
            int num_dim = dimensions.size();
            std::vector<std::string> vec_output = std::vector<std::string>();

            for (uint i = 0; i < num_dim; i++) {
                //std::cout << "wire_" << common_name << " - Dimension " << i << " seen as: [" << dimensions[i] << "]\n";
            }

            if (num_dim == 0) {
                //std::cout << common_name << " - Dimensions - NONE\n";
                std::stringstream ss_entry;
                ss_entry << "wire_" << common_name;
                vec_output.push_back(ss_entry.str());
            }
            else if ( num_dim == 1 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    std::stringstream ss_entry;
                    ss_entry << "wire_" << common_name << "[" << i << "]";
                    vec_output.push_back(ss_entry.str());
                }
            }
            else if ( num_dim == 2) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        std::stringstream ss_entry;
                        ss_entry << "wire_" << common_name << "[" << i << "][" << j << "]";
                        vec_output.push_back(ss_entry.str());
                    }
                }
            }
            else if ( num_dim == 3) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            std::stringstream ss_entry;
                            ss_entry << "wire_" << common_name << "[" << i << "][" << j << "][" << k << "]";
                            vec_output.push_back(ss_entry.str());
                        }
                    }
                }
            }
            else if ( num_dim == 4 ) {
                for (uint i = 0; i < dimensions[0]; i++) {
                    for (uint j = 0; j < dimensions[1]; j++) {
                        for (uint k = 0; k < dimensions[2]; k++) {
                            for (uint l = 0; l < dimensions[3]; l++) {
                                std::stringstream ss_entry;
                                ss_entry << "wire_" << common_name << "[" << i << "][" << j << "][" << k << "][" << l << "]" ;
                                vec_output.push_back(ss_entry.str());
                            }
                        }
                    }
                }
            }
            else {
                std::cerr << "\nERROR: wrapper_gen::tl_conn::make_unrolled_wires: Program doesn't understand a number of dimensions outside 0-4\n\n";
            }

            return vec_output;
        }

        // Creates the connections between the top level registers and the corresponding unrolled wrapper registers
        std::optional<std::vector<std::string>> make_wrapper_tl_conn(
            const std::vector<std::string> &tl_names, const std::vector<std::string> &wr_names, const std::vector<int> &dimensions
        ){
            std::vector<std::string> vec_output;
            int num_dim = dimensions.size();

            // check if sizes are equal, if not return error
            if ( tl_names.size() != wr_names.size() ) {
                std::cerr << "\nERROR: wrapper_gen::tl_conn::tl_io_connected: Encounted mismatch between the number of top level registers and wrapper registers for an one common name\n";
                std::cerr << "\tio size: " << tl_names.size() << ", expl size: " << wr_names.size() << "\n\n";
                return {};
            }

            uint num_indices = 1;
            switch (num_dim) {
                case 4 : num_indices *= dimensions[3]; // dimensions[0] * dimensions[1] * dimensions[2] * dimensions[3];
                case 3 : num_indices *= dimensions[2]; // dimensions[0] * dimensions[1] * dimensions[2]
                case 2 : num_indices *= dimensions[1]; // dimensions[0] * dimensions[1]
                case 1 : num_indices *= dimensions[0]; // dimensions[0]
                case 0 : break;
                default : {
                    std::cerr << "\nERROR: wrapper_gen::tl_conn::tl_io_connected: This program is not designed handle a number of dimensions outside 0-4\n";
                    return {};
                }
            }

            // connection time bois!
            for (uint i = 0; i < num_indices; i++) {
                std::stringstream ss_entry;
                ss_entry << "." << tl_names[i] << "(" << wr_names[i] << "),\n";
                vec_output.push_back(ss_entry.str());
            }

            // No errors, return 0
            return vec_output;
        }
    }

    namespace shift_logic {
        std::string make_shift_in(
            const std::string &common_name, const std::vector<int> &dimensions
        ){
            std::stringstream ss_output;
            int num_dim = dimensions.size();

            std::stringstream feed_in;
            std::stringstream hold_vals;

            std::stringstream feed_in_logic;
            std::stringstream hold_vals_logic;

            std::stringstream for_loops;
            std::stringstream for_loop_ends;

            std::stringstream logic_tabbing;

            feed_in << "if (feed_in_" << common_name << " == 1'h1) begin\n";
            hold_vals << "end else begin\n";


            switch (num_dim) {
                case 0 : {
                    for_loops << "";

                    for_loop_ends << "";

                    logic_tabbing << "\t";

                    feed_in_logic << logic_tabbing.str() << "reg_" << common_name << " <= bus_" << common_name << ";\n";

                    hold_vals << logic_tabbing.str() << "reg_" << common_name << " <= reg_" << common_name << ";\n";

                    break;
                }
                case 1 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";

                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t";

                    feed_in_logic << logic_tabbing.str() << "if (i==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\treg_" << common_name << "[i] <= bus_" << common_name << ";\n";
                    feed_in_logic << logic_tabbing.str() << "end else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\treg_" << common_name << "[i] <= reg_" << common_name << "[i-1];\n";
                    feed_in_logic << logic_tabbing.str() << "end\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i] <= reg_" << common_name << "[i];\n";

                    break;
                }
                case 2 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";

                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t";

                    feed_in_logic << logic_tabbing.str() << "if (j==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\tif (i==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j] <= bus_" << common_name << ";\n";
                    feed_in_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\tend\n";
                    feed_in_logic << logic_tabbing.str() << "end else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j] <= reg_" << common_name << "[i][j-1];\n";
                    feed_in_logic << logic_tabbing.str() << "end\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j] <= reg_" << common_name << "[i][j];\n";

                    break;
                }
                case 3 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";

                    for_loop_ends << "\t\t\tend\n";
                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t\t";

                    feed_in_logic << logic_tabbing.str() << "if (k==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\tif (j==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tif (i==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k] <= bus_" << common_name << ";\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1][" << dimensions[2] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tend\n";
                    feed_in_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j-1][" << dimensions[2] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\tend\n";
                    feed_in_logic << logic_tabbing.str() << "end else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j][k-1];\n";
                    feed_in_logic << logic_tabbing.str() << "end\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j][k];\n";

                    break;
                }
                case 4 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";
                    for_loops << "\t\t\t\tfor (l=0; l<" << dimensions[3] << "; l=l+1) begin\n";

                    for_loop_ends << "\t\t\t\tend\n";
                    for_loop_ends << "\t\t\tend\n";
                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t\t\t";

                    feed_in_logic << logic_tabbing.str() << "if (l==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\tif (k==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tif (j==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\tif (i==0) begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\t\treg_" << common_name << "[i][j][k][l] <= bus_" << common_name << ";\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1]["
                                << dimensions[2] << "-1][" << dimensions[3] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\tend\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j-1][" << dimensions[2]
                                << "-1][" << dimensions[3] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\t\tend\n";
                    feed_in_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k-1][" << dimensions[3] << "-1];\n";
                    feed_in_logic << logic_tabbing.str() << "\tend\n";
                    feed_in_logic << logic_tabbing.str() << "end else begin\n";
                    feed_in_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k][l-1];\n";
                    feed_in_logic << logic_tabbing.str() << "end\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k][l];\n";

                    break;
                }
                default : {
                    std::cerr << "\nERROR: wrapper_gen::shift_logic::make_shift_in: Number of dimensions is not supported!\n\n";
                }
            }

            feed_in << for_loops.str() << feed_in_logic.str() << for_loop_ends.str();
            hold_vals << for_loops.str() << hold_vals_logic.str() << for_loop_ends.str();
            hold_vals << "end\n";

            // write to output
            ss_output << feed_in.str() << hold_vals.str();

            return ss_output.str();
        }

        std::string make_shift_out(
            const std::string &common_name, const std::vector<int> &dimensions
        ){
            std::stringstream ss_output;
            int num_dim = dimensions.size();

            std::stringstream feed_out;
            std::stringstream hold_vals;
            std::stringstream load_vals;

            std::stringstream feed_out_logic;
            std::stringstream hold_vals_logic;
            std::stringstream load_vals_logic;

            std::stringstream for_loops;
            std::stringstream for_loop_ends;

            std::stringstream logic_tabbing;

            feed_out << "if (feed_out_" << common_name << " == 1'h1) begin\n";
            load_vals << "end else if (load_out_" << common_name << " == 1'h1) begin\n";
            hold_vals << "end else begin\n";


            switch (num_dim) {
                case 0 : {
                    for_loops << "";

                    for_loop_ends << "";

                    logic_tabbing << "\t";

                    feed_out_logic << logic_tabbing.str() << "bus_" << common_name << " <= reg_" << common_name << ";\n";

                    load_vals_logic << logic_tabbing.str() << "reg_" << common_name << " <= wire_" << common_name << ";\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << " <= reg_" << common_name << ";\n";

                    break;
                }
                case 1 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";

                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t";

                    feed_out_logic << logic_tabbing.str() << "if (i==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\treg_" << common_name << "[i] <= bus_" << common_name << ";\n";
                    feed_out_logic << logic_tabbing.str() << "end else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\treg_" << common_name << "[i] <= reg_" << common_name << "[i-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (i==" << dimensions[0] << "-1) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tbus_" << common_name << " <= reg_" << common_name << "[i];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end\n";

                    load_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i] <= wire_" << common_name << "[i];\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i] <= reg_" << common_name << "[i];\n";            

                    break;
                }
                case 2 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";

                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t";

                    feed_out_logic << logic_tabbing.str() << "if (j==0) begin\n";            
                    feed_out_logic << logic_tabbing.str() << "\tif (i==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j] <= bus_" << common_name << ";\n";
                    feed_out_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j] <= reg_" << common_name << "[i][j-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (j==" << dimensions[1] << "-1 && i==" << dimensions[0] << "-1) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tbus_" << common_name << " <= reg_" << common_name << "[i][j];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end\n";

                    load_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j] <= wire_" << common_name << "[i][j];\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j] <= reg_" << common_name << "[i][j];\n";

                    break;
                }
                case 3 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";

                    for_loop_ends << "\t\t\tend\n";
                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t\t";

                    feed_out_logic << logic_tabbing.str() << "if (k==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (j==0) begin\n";            
                    feed_out_logic << logic_tabbing.str() << "\t\tif (i==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k] <= bus_" << common_name << ";\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1][" << dimensions[2] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tend\n";
                    feed_out_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j-1][" << dimensions[2] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j][k-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (k==" << dimensions[2] << "-1 && j==" << dimensions[1] << "-1 && i==" << dimensions[0] << "-1) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tbus_" << common_name << " <= reg_" << common_name << "[i][j][k];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end\n";

                    load_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k] <= wire_" << common_name << "[i][j][k];\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k] <= reg_" << common_name << "[i][j][k];\n";

                    break;
                }
                case 4 : {
                    for_loops << "\tfor (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\t\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";
                    for_loops << "\t\t\t\tfor (l=0; l<" << dimensions[3] << "; l=l+1) begin\n";

                    for_loop_ends << "\t\t\t\tend\n";
                    for_loop_ends << "\t\t\tend\n";
                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";

                    logic_tabbing << "\t\t\t\t\t";

                    feed_out_logic << logic_tabbing.str() << "if (l==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (k==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tif (j==0) begin\n";            
                    feed_out_logic << logic_tabbing.str() << "\t\t\tif (i==0) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\t\treg_" << common_name << "[i][j][k][l] <= 0;\n"; //bus_" << common_name << ";\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i-1][" << dimensions[1] << "-1]["
                                                                                                            << dimensions[2] << "-1][" << dimensions[3] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\tend\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j-1][" << dimensions[2] 
                                                                                                                                << "-1][" << dimensions[3] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tend\n";
                    feed_out_logic << logic_tabbing.str() << "\tend else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k-1][" << dimensions[3] << "-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end else begin\n";
                    feed_out_logic << logic_tabbing.str() << "\treg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k][l-1];\n";
                    feed_out_logic << logic_tabbing.str() << "\tif (l==" << dimensions[3] << "-1 && k==" << dimensions[2] << "-1 && j==" << dimensions[1] << "-1 && i==" << dimensions[0] << "-1) begin\n";
                    feed_out_logic << logic_tabbing.str() << "\t\tbus_" << common_name << " <= reg_" << common_name << "[i][j][k][l];\n";
                    feed_out_logic << logic_tabbing.str() << "\tend\n";
                    feed_out_logic << logic_tabbing.str() << "end\n";

                    load_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k][l] <= wire_" << common_name << "[i][j][k][l];\n";

                    hold_vals_logic << logic_tabbing.str() << "reg_" << common_name << "[i][j][k][l] <= reg_" << common_name << "[i][j][k][l];\n";

                    break;
                }
                default : {
                    std::cerr << "\nERROR: wrapper_gen::shift_logic::make_shift_out: Number of dimensions is not supported!\n\n";
                }
            }

            feed_out << for_loops.str() << feed_out_logic.str() << for_loop_ends.str();
            load_vals << for_loops.str() << load_vals_logic.str() << for_loop_ends.str();
            hold_vals << for_loops.str() << hold_vals_logic.str() << for_loop_ends.str();
            hold_vals << "\tbus_" << common_name << " <= bus_" << common_name << ";\n";
            hold_vals << "end\n";

            // write to output
            ss_output << feed_out.str() << load_vals.str() << hold_vals.str();

            return ss_output.str();
        }
    }

    namespace reset_logic {
        std::string make_reg_reset(
                /* inputs  */ const std::string &common_name, const std::vector<int> &dimensions, const parsed_IO::reg_io_type &reg_status
        ){
            std::stringstream ss_output;
            int num_dim = dimensions.size();

            std::stringstream for_loops;
            std::stringstream for_loop_ends;

            std::stringstream clear_tabbing;
            std::stringstream clear_logic;

            switch (num_dim) {
                case 0 : {
                    for_loops << "";

                    for_loop_ends << "";

                    clear_tabbing << "";

                    clear_logic << clear_tabbing.str() << "reg_" << common_name << " <= 0;\n";

                    break;
                }
                case 1 : {
                    for_loops << "for (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";

                    for_loop_ends << "end\n";

                    clear_tabbing << "\t";

                    clear_logic << clear_tabbing.str() << "reg_" << common_name << "[i] <= 0;\n";

                    break;
                }
                case 2 : {
                    for_loops << "for (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";

                    for_loop_ends << "\tend\n";
                    for_loop_ends << "end\n";

                    clear_tabbing << "\t\t";

                    clear_logic << clear_tabbing.str() << "reg_" << common_name << "[i][j] <= 0;\n";

                    break;
                }
                case 3 : {
                    for_loops << "for (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";

                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";
                    for_loop_ends << "end\n";

                    clear_tabbing << "\t\t\t";

                    clear_logic << clear_tabbing.str() << "reg_" << common_name << "[i][j][k] <= 0;\n";

                    break;
                }
                case 4 : {
                    for_loops << "for (i=0; i<" << dimensions[0] << "; i=i+1) begin\n";
                    for_loops << "\tfor (j=0; j<" << dimensions[1] << "; j=j+1) begin\n";
                    for_loops << "\t\tfor (k=0; k<" << dimensions[2] << "; k=k+1) begin\n";
                    for_loops << "\t\t\tfor (l=0; l<" << dimensions[3] << "; l=l+1) begin\n";

                    for_loop_ends << "\t\t\tend\n";
                    for_loop_ends << "\t\tend\n";
                    for_loop_ends << "\tend\n";
                    for_loop_ends << "end\n";

                    clear_tabbing << "\t\t\t\t";

                    clear_logic << clear_tabbing.str() << "reg_" << common_name << "[i][j][k][l] <= 0;\n";

                    break;
                }
                default : {
                    std::cerr << "\nERROR: wrapper_gen::reset_logic::make_reg_reset: Number of dimensions is not supported!\n\n";
                }
            }

            ss_output << for_loops.str() << clear_logic.str() << for_loop_ends.str();

            if (reg_status == parsed_IO::OUTPUT_REG) {
                ss_output << "bus_" << common_name << " <= 0;\n";
            }

            return ss_output.str();

        }
    }
}

#endif // _WRAPPER_H_