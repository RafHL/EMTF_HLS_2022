pub mod print_controls {
    #[allow(dead_code)]
    pub struct PCtrlStruct {
        ctrl_bits: u8,
    }
    #[allow(dead_code)]
    impl PCtrlStruct {
        #[allow(non_upper_case_globals)] pub const CTRLBITS_FilePath: u8     = 0x01;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_FileAllLines: u8 = 0x02;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_FileIOLines: u8  = 0x04;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_BusNobus: u8     = 0x08;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_BusVec: u8       = 0x10;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_CurrPath: u8     = 0x20;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_IOData: u8       = 0x40;
        #[allow(non_upper_case_globals)] pub const CTRLBITS_StrDump: u8      = 0x80;
        
        pub fn new() -> PCtrlStruct {
            return PCtrlStruct { ctrl_bits: 0 }
        }

        pub fn set_ctrl(&mut self, cfg: u8) -> PCtrlStruct {
            return PCtrlStruct { ctrl_bits: self.ctrl_bits | cfg }
        }

        pub fn read_ctrl(&self, cfg: u8) -> bool {
            return (self.ctrl_bits & cfg) != 0;
        }
    }
}