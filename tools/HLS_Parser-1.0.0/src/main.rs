//use std::fs::File;
use std::io::prelude::*;

mod print_ctrl;
use crate::print_ctrl::print_controls::*;

mod structs;
use crate::structs::lines::*;
//use crate::structs::conversion::*;

mod processing;
use crate::processing::file::*;
use crate::processing::lines::*;
use crate::processing::conversion::*;


fn main() {
    // Print Control Setup, controls which print outs are active
    let print_controller = PCtrlStruct::set_ctrl(&mut PCtrlStruct::new(),
        // PCtrlStruct::CTRLBITS_FilePath |
        // PCtrlStruct::CTRLBITS_FileAllLines |
        // PCtrlStruct::CTRLBITS_FileIOLines |
        // PCtrlStruct::CTRLBITS_BusNobus |
        // PCtrlStruct::CTRLBITS_BusVec |
        // PCtrlStruct::CTRLBITS_CurrPath |
        // PCtrlStruct::CTRLBITS_IOData |
        PCtrlStruct::CTRLBITS_StrDump |
        0
    );


    // Grab Argument (file path)
    let args: Vec<String> = std::env::args().collect();

    // Check filepath (both as system and local)
    let filepath_string = args[1].clone();

    // Open file from filepath
    #[allow(unused_variables)]
    let file = file_opener(filepath_string).unwrap_or_else(|error| {
        panic!("ERROR: (main) - Failed to open file!");
    });

    // Load file into vector of strings (by line)
    let buf = std::io::BufReader::new(file);
    let file_lines : Vec<String> = buf.lines()
        .map(|l| l.expect("Could not parse line"))
        .collect();

    // Find inputs
    let file_inputs_raw: Vec<String>;
    match find_input_lines(&file_lines) {
        Some(found_inputs) => file_inputs_raw = found_inputs,
        None => panic!("ERROR: (main) - No Inputs Found!"),
    }
    let file_inputs: Vec<String> = remove_input_decl(file_inputs_raw);

    // Find outputs
    let file_outputs_raw: Vec<String>;
    match find_output_lines(&file_lines) {
        Some(found_inputs) => file_outputs_raw = found_inputs,
        None => panic!("ERROR: (main) - No Outputs Found!"),
    }

    let file_outputs: Vec<String> = remove_output_decl(file_outputs_raw);

    // Print
    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_FileIOLines) {
        for line in file_inputs.iter() {
            println!("Seen Input: \"{}\"", line);
        }
        for line in file_outputs.iter() {
            println!("Seen Output: \"{}\"", line);
        }
    }

    
    // Split inputs/outputs into sub categories, bus and no bus
    let thread_input_bus_vec = std::thread::spawn(move || {
        return split_bus_nobus_io(file_inputs);
    });
    let thread_output_bus_vec = std::thread::spawn(move || {
        return split_bus_nobus_io(file_outputs);
    });
    let input_bus_nobus: bus_nobus = thread_input_bus_vec.join().unwrap(); //split_bus_nobus_io(file_inputs);
    let output_bus_nobus: bus_nobus = thread_output_bus_vec.join().unwrap(); //split_bus_nobus_io(file_outputs);

    // Print
    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_BusNobus) {
        input_bus_nobus.print();
        output_bus_nobus.print();
    }


    // Split IO between bus w/ vec, non-bus /w vec, bus w/o vec, and non-bus w/o vec
    let thread_input_bus_vec = std::thread::spawn(move || {
        return cull_unneccessary_io(cull_indentifier_indices(split_vec_nvec(input_bus_nobus)));
    });
    let thread_output_bus_vec = std::thread::spawn(move || {
        return cull_unneccessary_io(cull_indentifier_indices(split_vec_nvec(output_bus_nobus)));
    });
    let input_bus_vec = thread_input_bus_vec.join().unwrap();
    let output_bus_vec = thread_output_bus_vec.join().unwrap();

    // Print
    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_BusVec) {
        input_bus_vec.print();
        output_bus_vec.print();
    }


    // Convert to "IO_Data"
    let thread_input_io_data = std::thread::spawn(move || {
        return to_io_data(input_bus_vec);
    });
    let thread_output_io_data = std::thread::spawn(move || {
        return to_io_data(output_bus_vec);
    });

    let input_io_data = thread_input_io_data.join().unwrap();
    let output_io_data = thread_output_io_data.join().unwrap();

    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_IOData) {
        input_io_data.iter().for_each(|elemn| { elemn.print() });
        output_io_data.iter().for_each(|elemn| { elemn.print() });
    }


    // Create Dump Strings for file output
    let string_input_dump = dump_IOData_vec(input_io_data);
    let string_output_dump = dump_IOData_vec(output_io_data);

    // Print Dump Strings
    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_StrDump) {
        print!("<<Inputs>>\n{}", string_input_dump);
        print!("<<Outputs>>\n{}", string_output_dump);
    }


    // Print the current path
    if print_controller.read_ctrl(PCtrlStruct::CTRLBITS_CurrPath) {
        let curr_path: String = std::env::current_dir().unwrap()
                                        .into_os_string().into_string().unwrap();
        println!("Current Dir: \"{}\"", curr_path);
    }
    
}

