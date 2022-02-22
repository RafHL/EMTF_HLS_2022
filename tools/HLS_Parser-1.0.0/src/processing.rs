// Functions related to file loading
pub mod file {
    // File opener function
    pub fn file_opener(filepath_string: String) -> Result<std::fs::File, std::io::Error> {
        //println!("Seen input is: \"{}\"", filepath_string);

        // Check if file exits
        let filepath = match std::fs::canonicalize(
            std::path::PathBuf::from(filepath_string)
        ) {
            Ok(path) => {
                //println!("Detected path is: {}", path.clone().into_os_string().into_string().unwrap());
                path
            },
            Err(e) => {
                println!("ERROR: (file_opener) - File Undetected!");
                return Err(e);
            },
        };

        // Load file
        let _f = match std::fs::File::open(filepath) {
            Ok(_f) =>  {
                return Ok(_f);
            },
            Err(e) => {
                println!("ERROR: (file_opener) - File Unloadable!");
                return Err(e);
            },
        };
    }

    // IOData to file
    /*
    use crate::structs::conversion::*;
    use crate::processing::conversion::*;
    pub fn write_IOData_to_file(data: Vec<IOData>) {
        let path = std::path::Path::new("parser_output.txt");
        let display = path.display();

        // Open a file in write-only mode, returns `io::Result<File>`
        let mut file = match std::fs::File::create(&path) {
            Err(why) => panic!("couldn't create {}: {}", display, why),
            Ok(file) => file,
        };

    }
    */
}

// Functions relating to processes lines
pub mod lines {
    use crate::structs::lines::*;

    // Finds lines corresponding to inputs
    pub fn find_input_lines(file_lines: &Vec<String>) -> Option<Vec<String>> {
        // Closure for checking if a string is an input line
        let input_check_closure = |line_to_check: &String| -> Option<String> {
            let no_leading_whitespace = line_to_check.trim_start().to_string();
            if no_leading_whitespace.len() >= 6 {
                if &no_leading_whitespace[0..5] == "input" {
                    return Some(no_leading_whitespace);
                }
                else { return None; }
            }
            else { return None; }
        };

        // Input Line Checking Loop
        let ret_vec = file_lines.iter()
                                .filter_map(|file_line| { input_check_closure(file_line) } )                                  
                                .collect::<Vec<String>>();

        
        // If there are inputs, return them
        if ret_vec.is_empty() { return None; } 
        else                  { return Some(ret_vec); }
    }

    pub fn find_output_lines(file_lines: &Vec<String>) -> Option<Vec<String>> {
        // Closure for checking if a string is an output line
        let output_check_closure = |line_to_check: &String| -> Option<String> {
            let no_leading_whitespace = line_to_check.trim_start().to_string();
            if no_leading_whitespace.len() >= 7 {
                if &no_leading_whitespace[0..6] == "output" {
                    return Some(no_leading_whitespace);
                }
                else { return None; }
            }
            else { return None; }
        };

        // Output Line Checking Loop
        let ret_vec = file_lines.iter()
                                .filter_map(|file_line| { output_check_closure(file_line) } )                                  
                                .collect::<Vec<String>>();

        
        // If there are outputs, return them
        if ret_vec.is_empty() { return None; } 
        else                  { return Some(ret_vec); }
    }

    // Removes the "input" declaration
    pub fn remove_input_decl(file_inputs: Vec<String>) -> Vec<String> {
        let ret_vec = file_inputs.iter()
                                 .map(|input_line| { 
                                     return input_line[5..].trim_start().to_string() })
                                 .collect::<Vec<String>>();
        return ret_vec;
    }

    // Removes the "output" declaration
    pub fn remove_output_decl(file_outputs: Vec<String>) -> Vec<String> {
        let ret_vec = file_outputs.iter()
                                  .map(|output_line| { 
                                      return output_line[6..].trim_start().to_string() })
                                  .collect::<Vec<String>>();
        return ret_vec;
    }

    // Splits io into two vectors, one that has lines which have a bus and one that has those that don't have a bus
    // must already be seperated into Input and Output groups which are to be processed independently (i.e. call 
    // this func twiced)
    pub fn split_bus_nobus_io(file_io: Vec<String>) -> bus_nobus {
        // Organizational Enum
        enum IsBus {
            Bus([String; 2]),
            NBus(String),
            Invld,
        }
    
        // Closure that checks if an IO line has a bus (this is used inefficiently in the loops)
        let bus_check_closure = |line: &String| -> IsBus {
            let input_words = line.split(" ").map(|elemn| {elemn.to_string()}).collect::<Vec<String>>();

            // Has bus
            if input_words[0].contains("[") && input_words[0].contains(":") && input_words[0].contains("]") {
                if input_words.len() != 2 { panic!("Expected Line w/ Bus to have name!"); } // Contegency Panic

                // index 0 should contain the string corresponding to the bus, index 1 should have the identifier
                let some_size = input_words[1].len();
                if some_size > 3 {
                   return IsBus::Bus([input_words[0].clone(), input_words[1][..(some_size-3)].to_string()]); // gets rid of "_V;"
                }
                else { return IsBus::Invld; } // Format is not correct
            }

            // No bus
            else { 
                // (ignore any io w/ "_ap_vld")
                if !(line.contains("_ap_vld")) && (line.len() > 1) {
                    return IsBus::NBus(line[..line.len()-1].to_string());
                }
                else { return IsBus::Invld; } // has "_ap_vld"
            }
        };

        // Main Loops
        let io_has_bus = file_io.iter()
                                .filter_map(|io_line| { 
                                    match bus_check_closure(io_line) {
                                        IsBus::Bus(bus_stuff) => { return Some(bus_stuff) },
                                        _ => return None,
                                    }
                                })
                                .collect::<Vec<[String; 2]>>();

        let io_no_bus = file_io.iter()
                               .filter_map(|io_line| { 
                                   match bus_check_closure(io_line) {
                                       IsBus::NBus(nbus_stuff) => { return Some(nbus_stuff) },
                                       _ => return None,
                                   }
                                })
                               .collect::<Vec<String>>();

        // Wrap bus data
        let ret_has_bus: Option<Vec<[String; 2]>>;
        let ret_no_bus: Option<Vec<String>>;

        if io_has_bus.is_empty() { ret_has_bus = None; }
        else { ret_has_bus = Some(io_has_bus); }

        if io_no_bus.is_empty() { ret_no_bus = None; }
        else { ret_no_bus = Some(io_no_bus); }

        // Condense bus data into a bus_nobus struct
        let ret_struct: bus_nobus = bus_nobus { 
            has_bus: ret_has_bus, 
            no_bus: ret_no_bus 
        };

        return ret_struct;
    }

    pub fn split_vec_nvec( io_bus_nobus: bus_nobus ) -> bus_vec {
        // Useful Closures
        let detect_indices_closure = |io_name: String| -> bool {
            // breakup Identifier by "_"
            let fragments: Vec<&str> = io_name.split("_").collect();
            let mut found_index = false;

            for fragment in fragments.iter() {
                match fragment.parse::<usize>() {
                    Ok(_) => { found_index = true; break; }, // add index
                    _ => {},
                }
            }

            return found_index;
        };
        let get_indices_closure = |io_name: String| -> Vec<usize> {
            // breakup Identifier by "_"
            let fragments: Vec<&str> = io_name.split("_").collect();

            let indices = fragments.iter()
                                   .filter_map(|fragment| {
                                       return match fragment.parse::<usize>() {
                                           Ok(index) => Some(index),
                                           _ => None,
                                       }
                                   }).collect::<Vec<usize>>();

            return indices;
        };


        // Major variables
        let ret_struct: bus_vec;
        let ret_bus_vec: Option<Vec<([String; 2], Vec<usize>)>>;
        let ret_nbus_vec: Option<Vec<(String, Vec<usize>)>>;
        let ret_bus_nvec: Option<Vec<[String; 2]>>;
        let ret_nbus_nvec: Option<Vec<String>>;

        let intr_bus_vec: Vec<([String; 2], Vec<usize>)>;
        let intr_nbus_vec: Vec<(String, Vec<usize>)>;
        let intr_bus_nvec: Vec<[String; 2]>;
        let intr_nbus_nvec: Vec<String>;


        // Main Loops
        // Get IO w/ bus split into vec and non-vec
        if io_bus_nobus.has_bus != None {
            // Split IO based on whether there are numerical indices inside it's Identifier's Name Section
            let ( have_indices, no_indices ): (Vec<[String; 2]>, Vec<[String; 2]>) 
                = io_bus_nobus.has_bus.unwrap()
                                      .into_iter()
                                      .partition(|identifier| { 
                                          return detect_indices_closure(identifier[1].to_string());
                                      });
            
            // Process the IO with indices
            intr_bus_vec = have_indices.iter().map(|identifier| {
                return (identifier.clone(), get_indices_closure(identifier[1].to_string()))
            }).collect();

            // Pass IO w/o indices
            intr_bus_nvec = no_indices;
        }
        else {
            intr_bus_vec = Vec::new();
            intr_bus_nvec = Vec::new();
        }
        // Get IO w/o bus split into vec and non-vec
        if io_bus_nobus.no_bus != None {
            // Split IO based on whether there are numerical indices inside it's Identifier's Name Section
            let ( have_indices, no_indices ): (Vec<String>, Vec<String>) 
                = io_bus_nobus.no_bus.unwrap()
                                     .into_iter()
                                     .partition(|identifier| { 
                                         return detect_indices_closure(identifier.to_string());
                                     });
            
            // Process the IO with indices
            intr_nbus_vec = have_indices.iter().map(|identifier| {
                return (identifier.clone(), get_indices_closure(identifier.to_string()))
            }).collect();

            // Pass IO w/o indices
            intr_nbus_nvec = no_indices;
        }
        else {
            intr_nbus_vec = Vec::new();
            intr_nbus_nvec = Vec::new();
        }


        // Wrap Intermediary
        if intr_bus_vec.is_empty() { ret_bus_vec = None; }
        else { ret_bus_vec = Some(intr_bus_vec); }

        if intr_bus_nvec.is_empty() { ret_bus_nvec = None; }
        else { ret_bus_nvec = Some(intr_bus_nvec); }

        if intr_nbus_vec.is_empty() { ret_nbus_vec = None; }
        else { ret_nbus_vec = Some(intr_nbus_vec); }

        if intr_nbus_nvec.is_empty() { ret_nbus_nvec = None; }
        else { ret_nbus_nvec = Some(intr_nbus_nvec); }


        // Ret to output struct
        ret_struct = bus_vec {
            bus_vec: ret_bus_vec, // IO has a bus and is part of a vector
            nbus_vec: ret_nbus_vec, // IO has no bus and is part of a vector
            bus_nvec: ret_bus_nvec, // IO has a bus and is not part of a vector
            nbus_nvec: ret_nbus_nvec, // IO has no bus and is not part of a vector
        };

        return ret_struct;
    }

    // Remove indentifier indices
    pub fn cull_indentifier_indices(io_bus_vec: bus_vec) -> bus_vec {
        // Useful Closure
        let remove_indices_closure = |io_name: String| -> String {
            // Remove fragements that contain indices
            let culled_fragments = io_name.split("_")
                                          .filter_map(|fragment| {
                                              match fragment.parse::<usize>() {
                                                  Ok(_) => None,
                                                  Err(_) => Some(fragment.to_string())
                                              }
                                          })
                                          .collect::<Vec<String>>();
            
            // Reconstruct io_name from the set of non-culled fragments
            let mut culled_io_name: String = String::new();
            let mut first_fragment = true;
            culled_fragments.iter().for_each(|fragment| {
                if !first_fragment {
                    culled_io_name.push_str("_")
                } else { first_fragment = false; }

                culled_io_name.push_str(fragment);
            });

            return culled_io_name;
        }; 


        // Major Variables
        let ret_struct: bus_vec;
        let ret_bus_vec: Option<Vec<([String; 2], Vec<usize>)>>;// = io_bus_vec.bus_vec.clone();
        let ret_nbus_vec: Option<Vec<(String, Vec<usize>)>>;// = io_bus_vec.nbus_vec.clone();
        let ret_bus_nvec: Option<Vec<[String; 2]>> = io_bus_vec.bus_nvec.clone();
        let ret_nbus_nvec: Option<Vec<String>> = io_bus_vec.nbus_nvec.clone();


        // Trim identifiers that are parts of vectors
        if io_bus_vec.bus_vec != None {
            
            let unwrapped_bus_vec = io_bus_vec.bus_vec.unwrap().iter().map(|io_info| {
                return ( [ io_info.0[0].clone(), remove_indices_closure(io_info.0[1].clone()) ], io_info.1.clone() );
            }).collect::<Vec<([String; 2], Vec<usize>)>>();

            ret_bus_vec = Some(unwrapped_bus_vec);
        }
        else { ret_bus_vec = None; }

        if io_bus_vec.nbus_vec != None {
            
            let unwrapped_nbus_vec = io_bus_vec.nbus_vec.unwrap().iter().map(|io_info| {
                return ( remove_indices_closure(io_info.0.clone()), io_info.1.clone() );
            }).collect::<Vec<(String, Vec<usize>)>>();

            ret_nbus_vec = Some(unwrapped_nbus_vec);
        }
        else { ret_nbus_vec = None; }

        
        // Dump to the Return Struct
        ret_struct = bus_vec {
            bus_vec: ret_bus_vec,
            nbus_vec: ret_nbus_vec,
            bus_nvec: ret_bus_nvec,
            nbus_nvec: ret_nbus_nvec,
        };

        return ret_struct;
    }

    // We don't need all of the IO Identifiers for each of the IO vectors.
    // Assumes the last appearance of a specific Identifier has the largest
    // vector indices.
    pub fn cull_unneccessary_io(io_raw_vec_bus: bus_vec) -> bus_vec {
        // Major Variables
        let ret_struct: bus_vec;
        let ret_bus_vec: Option<Vec<([String; 2], Vec<usize>)>>;
        let ret_nbus_vec: Option<Vec<(String, Vec<usize>)>>;
        let ret_bus_nvec: Option<Vec<[String; 2]>>;
        let ret_nbus_nvec: Option<Vec<String>>;
        
        // Has a Bus and is part of a Vector
        if io_raw_vec_bus.bus_vec.is_some() {
            let mut prev_io: ([String; 2], Vec<usize>) = ([String::new(), String::new()], vec![]); // SIDE-EFFECT!!!
            let mut store = io_raw_vec_bus.bus_vec.unwrap().iter().filter_map(|io_data| {
                
                let ret_val: Option<([String; 2], Vec<usize>)>;
                if io_data.0[1] != prev_io.0[1] {
                    ret_val = Some(prev_io.clone());
                } else {
                    ret_val = None;
                }

                prev_io = io_data.clone();
                return ret_val;

            }).skip(1).collect::<Vec<([String; 2], Vec<usize>)>>();
            store.push(prev_io); // don't forget the last one

            ret_bus_vec = Some(store);
        }
        else { ret_bus_vec = None; }

        // Has no Bus and is part of a Vector
        if io_raw_vec_bus.nbus_vec.is_some() {
            let mut prev_io: (String, Vec<usize>) = (String::new(), vec![]); // SIDE-EFFECT!!!
            let mut store = io_raw_vec_bus.nbus_vec.unwrap().iter().filter_map(|io_data| {
                
                let ret_val: Option<(String, Vec<usize>)>;
                if io_data.0 != prev_io.0 {
                    ret_val = Some(prev_io.clone());
                } else {
                    ret_val = None;
                }

                prev_io = io_data.clone();
                return ret_val;

            }).skip(1).collect::<Vec<(String, Vec<usize>)>>();
            store.push(prev_io); // don't forget the last one

            ret_nbus_vec = Some(store);
        }
        else { ret_nbus_vec = None; }

        // Has no Vector (no need to trim)
        ret_bus_nvec = io_raw_vec_bus.bus_nvec;
        ret_nbus_nvec = io_raw_vec_bus.nbus_nvec;

        ret_struct = bus_vec {
            bus_vec: ret_bus_vec,
            nbus_vec: ret_nbus_vec,
            bus_nvec: ret_bus_nvec,
            nbus_nvec: ret_nbus_nvec,
        };

        return ret_struct;
    }

}

// Functions related to converting lines into IO data
pub mod conversion {
    use crate::structs::lines::*;
    use crate::structs::conversion::*;

    pub fn to_io_data(io: bus_vec) -> Vec<IOData> {
        // Conversion Closures, convert (n)bus_(n)vec data to IO Data form
        let bus_vec_conv_closure = |element: &([String; 2], Vec<usize>)| -> IOData {
            return IOData {
                signal_name: element.0[1].clone(),
                bus: Some(element.0[0].clone()),
                dimensions: Some(element.1.iter().map(|sub_elemn| { (*sub_elemn).clone()+1 }).collect::<Vec<usize>>()),
            }
        };
        let nbus_vec_conv_closure = |element: &(String, Vec<usize>)| -> IOData {
            return IOData {
                signal_name: element.0.clone(),
                bus: None,
                dimensions: Some(element.1.iter().map(|sub_elemn| { (*sub_elemn).clone()+1 }).collect::<Vec<usize>>()),
            }
        };
        let bus_nvec_conv_closure = |element: &[String; 2]| -> IOData {
            return IOData {
                signal_name: element[1].clone(),
                bus: Some(element[0].clone()),
                dimensions: None,
            }
        };
        let nbus_nvec_conv_closure = |element: &String| -> IOData {
            return IOData {
                signal_name: element.clone(),
                bus: None,
                dimensions: None,
            }
        };

        // Copy Data for thread inputs
        let thread_has_bus_vec_in_data = io.bus_vec.clone();
        let thread_has_nbus_vec_in_data = io.nbus_vec.clone();
        let thread_has_bus_nvec_in_data = io.bus_nvec.clone();
        let thread_has_nbus_nvec_in_data = io.nbus_nvec.clone();

        // Create Threads to Convert
        let thread_has_bus_vec = std::thread::spawn(move || {
            match thread_has_bus_vec_in_data {
                Some(has_bus_vec) => {
                    // Conversion Loop (bus_vec -> IO_Data)
                    let temp_vec = has_bus_vec.iter()
                                              .map(|elemn| -> IOData { return bus_vec_conv_closure(elemn); })
                                              .collect::<Vec<IOData>>();
                    return Some(temp_vec);
                },
                None => {None},
            }
        });
        let thread_has_nbus_vec = std::thread::spawn(move || {
            match thread_has_nbus_vec_in_data {
                Some(has_nbus_vec) => {
                    // Conversion Loop (nbus_vec -> IO_Data)
                    let temp_vec = has_nbus_vec.iter()
                                               .map(|elemn| -> IOData { return nbus_vec_conv_closure(elemn); })
                                               .collect::<Vec<IOData>>();
                    return Some(temp_vec);
                },
                None => {None},
            }
        });
        let thread_has_bus_nvec = std::thread::spawn(move || {
            match thread_has_bus_nvec_in_data {
                Some(has_bus_nvec) => {
                    // Conversion Loop (bus_nvec -> IO_Data)
                    let temp_vec = has_bus_nvec.iter()
                                               .map(|elemn| -> IOData { return bus_nvec_conv_closure(elemn); })
                                               .collect::<Vec<IOData>>();
                    return Some(temp_vec);
                },
                None => {None},
            }
        });
        let thread_has_nbus_nvec = std::thread::spawn(move || {
            match thread_has_nbus_nvec_in_data {
                Some(has_nbus_nvec) => {
                    // Conversion Loop (nbus_nvec -> IO_Data)
                    let temp_vec = has_nbus_nvec.iter()
                                                .map(|elemn| -> IOData { return nbus_nvec_conv_closure(elemn); })
                                                .collect::<Vec<IOData>>();
                    return Some(temp_vec);
                },
                None => {None},
            }
        });

        // Join threads and retrieve their data
        let tempvec_bus_vec = thread_has_bus_vec.join().unwrap();
        let tempvec_nbus_vec = thread_has_nbus_vec.join().unwrap();
        let tempvec_bus_nvec = thread_has_bus_nvec.join().unwrap();
        let tempvec_nbus_nvec = thread_has_nbus_nvec.join().unwrap();

        // Condense thread data
        let mut ret_vec: Vec<IOData> = vec![];
        if tempvec_bus_vec.is_some() { 
            tempvec_bus_vec.unwrap().iter().for_each(|elemn| { ret_vec.push((*elemn).clone()) });
        }
        if tempvec_nbus_vec.is_some() { 
            tempvec_nbus_vec.unwrap().iter().for_each(|elemn| { ret_vec.push((*elemn).clone()) });
        }
        if tempvec_bus_nvec.is_some() { 
            tempvec_bus_nvec.unwrap().iter().for_each(|elemn| { ret_vec.push((*elemn).clone()) });
        }
        if tempvec_nbus_nvec.is_some() { 
            tempvec_nbus_nvec.unwrap().iter().for_each(|elemn| { ret_vec.push((*elemn).clone()) });
        }

        // Return converted data
        return ret_vec;
    }

    #[allow(non_snake_case)]
    pub fn dump_IOData_vec(data: Vec<IOData>) -> String {
        let mut io_dumpstring: String = String::new();

        data.iter().for_each(|io| {
            // io name
            io_dumpstring.push_str("io_name(");
            io_dumpstring.push_str(&io.signal_name);
            io_dumpstring.push_str(") ");

            // bus
            if io.bus.is_some() {
                io_dumpstring.push_str("bus(y)(");
                io_dumpstring.push_str(&io.bus.clone().unwrap());
                io_dumpstring.push_str(") ");
            } else {
                io_dumpstring.push_str("bus(n) ");
            }

            // vec
            if io.dimensions.is_some() {
                io_dumpstring.push_str("vec(y)(");
                io_dumpstring.push_str(&(io.dimensions.clone().unwrap()[0].to_string()));
                io.dimensions.clone().unwrap().iter().skip(1).for_each(|dim| {
                    io_dumpstring.push_str(",");
                    io_dumpstring.push_str(&dim.to_string());
                });
                io_dumpstring.push_str(");\n");
            } else {
                io_dumpstring.push_str("vec(n);\n");
            }
        });

        return io_dumpstring;
    }

}