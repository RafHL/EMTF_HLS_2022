pub mod lines {
    // Struct for storing the lines that have a bus and those that do not
    #[derive(Clone)]
    #[allow(non_camel_case_types)]
    pub struct bus_nobus {
        pub has_bus: Option<Vec<[String; 2]>>,
        pub no_bus: Option<Vec<String>>,
    }
    impl bus_nobus {
        //#[allow(dead_code)]
        pub fn print(&self) {
            if self.has_bus != None {
                self.has_bus.clone().unwrap().iter().for_each(|an_io| {
                    println!("\"{}\" - Has Bus - \"{}\"", an_io[1], an_io[0]);
                });
            }
            if self.no_bus != None {
                self.no_bus.clone().unwrap().iter().for_each(|an_io| {
                    println!("\"{}\" - Has No Bus", an_io);
                });
            }
        }
    }

    // Struct to store IO that is non-bus w/ vec, bus w/ vec, non-bus w/o vec, and bus w/o vec
    #[derive(Clone)]
    #[allow(non_camel_case_types)]
    pub struct bus_vec {
        pub bus_vec: Option<Vec<([String; 2], Vec<usize>)>>,
        pub nbus_vec: Option<Vec<(String, Vec<usize>)>>,
        pub bus_nvec: Option<Vec<[String; 2]>>,
        pub nbus_nvec: Option<Vec<String>>,
    }
    impl bus_vec {
        //#[allow(dead_code)]
        pub fn print(&self) {
            if self.bus_vec != None {
                self.bus_vec.clone().unwrap().iter().for_each(|an_io| {
                    print!("\"{}\"\t - Has Bus - \"{}\"\t - Has Vec - < {}", an_io.0[1], an_io.0[0], an_io.1[0]);
                    an_io.1.iter().skip(1).for_each(|index| { print!(", {}", index) });
                    print!(" >\n");
                });
            }
            if self.nbus_vec != None {
                self.nbus_vec.clone().unwrap().iter().for_each(|an_io| {
                    println!("\"{}\"\t - Has No Bus\t - Has Vec - < {}", an_io.0, an_io.1[0]);
                    an_io.1.iter().skip(1).for_each(|index| { print!(", {}", index) });
                    print!(" >\n");
                });
            }
            if self.bus_nvec != None {
                self.bus_nvec.clone().unwrap().iter().for_each(|an_io| {
                    println!("\"{}\"\t - Has Bus - \"{}\"\t - Non Vec", an_io[1], an_io[0]);
                });
            }
            if self.nbus_nvec != None {
                self.nbus_nvec.clone().unwrap().iter().for_each(|an_io| {
                    println!("\"{}\"\t - No Bus\t - Non Vec", an_io);
                });
            }
        }
    }
}

pub mod conversion {
    // IO Data which has been converted from bus_vec data, chiefly for convenience 
    #[derive(Clone)]
    pub struct IOData {
        pub signal_name: String,
        pub bus: Option<String>,
        pub dimensions: Option<Vec<usize>>
    }
    impl IOData {
        #[allow(dead_code)]
        pub fn print(&self) {
            print!("name: {}", self.signal_name);

            print!("\t- bus: {}", match self.bus.clone() {
                Some(bus_stuff) => {bus_stuff},
                None => "NA".to_string(),
            });

            print!("\t- vec:");
            match self.dimensions.clone() {
                Some(s_unwraped) => {
                    s_unwraped.iter().for_each(|elemn| { print!(" {}", elemn); });
                },
                None => { print!(" NA"); },
            }
            print!("\n");
        }
    }
}