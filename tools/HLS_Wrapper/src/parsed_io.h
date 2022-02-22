#ifndef _MAKER_H_
#define _MAKER_H_

// Includes
#include <iostream>
#include <vector>
#include <array>
#include <sstream>


// Namespace Constants/Typedefs/Enums/Structs
namespace parsed_IO {
    enum reg_io_type {
        INPUT_REG = 0,
        OUTPUT_REG = 1
    };
    struct parsed_IO {
        std::string io_name;
        reg_io_type io_type;
        std::string io_bus;
        std::vector<int> io_dim;
    };

    // sub namespaces
    namespace parsed_data {
        // IO_VEC< IO_array > <== IO_array[IO_name, IO_bus, IO_vec]
        typedef std::vector<std::array<std::string, 3>> parsed_data_t;
    }
}


// Namespace Function Declarations
namespace parsed_IO {
    // Will return the io_name
    std::string get_io_name(const std::string &in_string);
    // Will return either the bus string, or a blank string
    std::string get_io_bus(const std::string &in_string);
    // Will return eithere the dimension vector, or a blank vector
    std::vector<int> get_io_dim(const std::string &in_string);
}
namespace parsed_IO::parsed_data {
    // [(input)IO_VEC, (output)IO_VEC]
    std::array<parsed_data_t, 2> from_string(const std::string &in_string );
    // IO_VEC ==> VEC<parsed_io>
    std::vector<parsed_IO> to_parsed_IO(parsed_data_t &data_in, const reg_io_type &curr_type);
}


// Namespace Function Definitions
namespace parsed_IO {
    std::string get_io_name(
        const std::string &in_string
    ){
        std::string ret_string = "";
        std::string std_name_wrap = "io_name(";

        // if name wrapper not found
        if (in_string.find(std_name_wrap) == std::string::npos) {
            std::cerr << "\nERROR: parsed_IO::get_io_name: no io_name wrapper\n\n";
            return ret_string;
        }

        // Get name
        bool name_trigger = false;
        for (auto c : in_string) {
            if (ret_string == std_name_wrap) {
                name_trigger = true;
                ret_string = "";
            }
            if (c == ')') {
                name_trigger = false;
                break;
            }

            ret_string += c;
        }
        if (name_trigger) {
            std::cerr << "\nERROR: parsed_IO::get_io_name: io_name wrapper not closed w/ \")\"\n\n";
        }

        return ret_string;
    }

    std::string get_io_bus(
        const std::string &in_string
    ){
        std::string ret_string = "";
        std::string std_bus_wrap_1 = "bus(";
        std::string std_bus_wrap_2 = ")(";

        // if name wrapper not found
        if (
            (in_string.find(std_bus_wrap_1) == std::string::npos) //||
            //(in_string.find(std_bus_wrap_2) == std::string::npos)
        ){
            std::cerr << "\nERROR: parsed_IO::get_io_bus: no/partial io_bus wrapper\n\n";
            return ret_string;
        }

        // Get bus (if any)
        enum get_bus_state {
            STATE_none,
            STATE_check_1,
            STATE_check_2,
            STATE_bus
        };
        get_bus_state loop_state = STATE_none;
        for (auto c : in_string) {
            switch (loop_state) {
                case STATE_none : {
                    ret_string += c;
                    if (ret_string == std_bus_wrap_1){
                        ret_string = "";
                        loop_state = STATE_check_1;
                    }
                    break;
                }
                case STATE_check_1 : {
                    ret_string = "";
                    if (c == 'y') { // Has bus
                        loop_state = STATE_check_2;
                    } else if (c == 'n') { // Doesn't have a bus
                        return ret_string;
                    } else {
                        std::cerr << "\nERROR: pased_IO::get_io_bus: invalid io_bus wrapper option {\"" << c << "\"}\n\n";
                        return ret_string;
                    }
                    break;
                }
                case STATE_check_2 : {
                    ret_string += c;
                    if (c != ')' && c != '(') {
                        std::cerr << "\nERROR: pased_IO::get_io_bus: no/partial io_bus wrapper\n\n";
                        return ret_string;
                    }
                    if (ret_string == std_bus_wrap_2) {
                        ret_string = "";
                        loop_state = STATE_bus;
                    }
                    break;
                }
                case STATE_bus : {
                    if (c == ')') {
                        return ret_string;
                    }
                    ret_string += c;
                    break;
                }
            }
        }

        std::cerr << "\nERROR: pased_IO::get_io_bus: io_bus wrapper not closed w/ \")\"\n\n";
        return ret_string;
    }

    std::vector<int> get_io_dim(
        const std::string &in_string
    ){
        std::vector<int> ret_vec;
        std::string std_vec_wrap_1 = "vec(";
        std::string std_vec_wrap_2 = ")(";

        // if name wrapper not found
        if (
                (in_string.find(std_vec_wrap_1) == std::string::npos) //||
                //(in_string.find(std_vec_wrap_2) == std::string::npos)
                ){
            std::cerr << "\nERROR: parsed_IO::get_io_vec: no/partial io_vec wrapper\n\n";
            return ret_vec;
        }

        // Get bus (if any)
        enum get_bus_state {
            STATE_none,
            STATE_check_1,
            STATE_check_2,
            STATE_vec
        };
        std::string temp_string = "";
        get_bus_state loop_state = STATE_none;
        for (auto c : in_string) {
            switch (loop_state) {
                case STATE_none : {
                    temp_string += c;
                    if (temp_string == std_vec_wrap_1){
                        temp_string = "";
                        loop_state = STATE_check_1;
                    }
                    break;
                }
                case STATE_check_1 : {
                    temp_string = "";
                    if (c == 'y') { // Has bus
                        loop_state = STATE_check_2;
                    } else if (c == 'n') { // Doesn't have a bus
                        return ret_vec;
                    } else {
                        std::cerr << "\nERROR: pased_IO::get_io_vec: invalid io_vec wrapper option {\"" << c << "\"}\n\n";
                        return ret_vec;
                    }
                    break;
                }
                case STATE_check_2 : {
                    temp_string += c;
                    if (c != ')' && c != '(') {
                        std::cerr << "\nERROR: parsed_IO::get_io_bus: no/partial io_bus wrapper\n\n";
                        return ret_vec;
                    }
                    if (temp_string == std_vec_wrap_2) {
                        temp_string = "";
                        loop_state = STATE_vec;
                    }
                    break;
                }
                case STATE_vec : {
                    if (c == ')') {
                        ret_vec.push_back(stoi(temp_string));
                        return ret_vec;
                    } else if (c == ',') {
                        ret_vec.push_back(stoi(temp_string));
                        temp_string = "";
                        break;
                    }
                    temp_string += c;
                    break;
                }
            }
        }

        std::cerr << "\nERROR: parsed_IO::get_io_vec: io_vec wrapper not closed w/ \")\"\n\n";
        return ret_vec;
    }
}
namespace parsed_IO::parsed_data {
    // converts parsed_data string into two parsed_data_t vectors, one for the inputs, and one for the outputs
    std::array<parsed_data_t, 2> from_string(
        const std::string &in_string
    ){
        // string to stringstream
        std::stringstream in_ss;
        in_ss << in_string;

        // Return data [(inputs)IO_VEC, (outputs)IO_VEC]
        std::array<parsed_data_t, 2> ret_data;

        // Useful enum
        enum loop_state {
            l_state_pre = 0,
            l_state_inputs = 1,
            l_state_outputs = 2
        };

        // Get data from string
        loop_state state = l_state_pre;
        volatile int word_index = 0;
        std::array<std::string, 3> *curr_vec_elemn;
        std::string current_word;
        while (in_ss >> current_word) {
            // Catch State Change
            if (current_word == "<<Inputs>>") { state = l_state_inputs; continue; }
            else if (current_word == "<<Outputs>>") { state = l_state_outputs; continue; }

            // Reserve vector entry for data
            if (word_index == 0) {
                // Push the array
                switch(state) {
                    case l_state_inputs:  {
                        ret_data[0].push_back({"", "", ""});
                        curr_vec_elemn = &ret_data[0].back();
                        break;
                    }
                    case l_state_outputs: {
                        ret_data[1].push_back({"", "", ""});
                        curr_vec_elemn = &ret_data[1].back();
                        break;
                    }
                }
            }

            // Capture word into the appropriate index
            (*curr_vec_elemn)[word_index] = current_word;
            word_index++;
            if (word_index == 3) { word_index = 0; }
        }

        return ret_data;
    }

    // converts from parsed_data_t to a vector of parsed_IO
    std::vector<parsed_IO> to_parsed_IO(
        parsed_data_t &data_in, const reg_io_type &curr_type
    ){
        // Return value
        std::vector<parsed_IO> ret_IO;

        parsed_data_t::iterator it;
        for ( it = data_in.begin(); it < data_in.end(); it++) {
            ret_IO.push_back({
                get_io_name((*it)[0]),
                curr_type,
                get_io_bus((*it)[1]),
                get_io_dim((*it)[2])
            });
        }

        return ret_IO;
    }
}


#endif // _MAKER_H_