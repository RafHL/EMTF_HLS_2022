# Makefile for testbench file. Includes all warnings besides unknown pragma,
# ints as bools, unused labels
# warnings
#
# Please direct any questions at me: rhernandezlopez1@ufl.edu
CC    = g++
FILES = tb/sp_tb.cc src/sp.cc src/prim_conv_sector.cc src/prim_conv.cc src/prim_conv11.cc src/zones.cc src/extender.cc
OPT   = -g -o sp_tb.out -Wall -Wno-unknown-pragmas -Wno-int-in-bool-context -Wno-unused-label -std=c++11

FILES_E = tb/sp_tb_EMTFe.cc src/sp_EMTFe.cc src/prim_conv_sector_EMTFe.cc src/prim_conv_EMTFe.cc src/prim_conv11_EMTFe.cc src/zones_EMTFe.cc src/extend_sector_EMTFe.cc src/extender_EMTFe.cc src/ph_pattern_sector_EMTFe.cc src/ph_pattern_EMTFe.cc
OPT_E   = -g -o sp_tb_EMTFe.out -Wall -Wno-unknown-pragmas -Wno-int-in-bool-context -Wno-unused-label -std=c++11
# TODO: Add coord_delay stuff from Andrew here
ZFILES = tb/sp_tb_EMTFe.cc tb/hex_to_ap.h tb/ap_to_hex.h tb/fill_params.h tops_Rafael/*EMTFe*.cc Makefile tcl/ emtf_data/ emtf-hlslib/ \
         src/sp_EMTFe.cc src/prim_*EMTFe.cc src/coord_delay.cc interface/sp_EMTFe.h interface/prim_*EMTFe.h interface/coord_delay*.h interface/spbits.h interface/ph_init_hard.h interface/zns_bnd.h include/
#ZFILES = tb/sp_tb_EMTFe.cc tb/hex_to_ap.h tb/ap_to_hex.h tb/fill_params.h src/*EMTFe.cc interface/*EMTFe.h interface/spbits.h interface/layer_sz.h interface/pbank.h interface/ph_init_hard.h interface/zns_bnd.h include/ tops_Rafael/*EMTFe*.cc Makefile tcl/ emtf_data/ emtf-hlslib/
ZIPFILE = hls_`date +'%Y-%m-%d-%T'`.zip

full:
	$(MAKE) csim
	$(MAKE) csynth
	#$(MAKE) cosim # Needs work at time of writing, Vivado bug seems to mess up tv/cdatafile reading
	$(MAKE) wrapper
	$(MAKE) wrp_synth

cosim:
	vivado_hls -f tcl/csynth_gold_r_ev.tcl
	vivado_hls -f tcl/cosim_r_ev.tcl

cosim_only:
	#vitis_hls -f tcl/cosim_r_ev.tcl
	vivado_hls -f tcl/cosim_r_ev.tcl

csim:
	#vitis_hls -f tcl/csim_r_ev.tcl
	vivado_hls -f tcl/csim_r_ev.tcl

csynth:
	#vitis_hls -f tcl/csynth_r_ev.tcl
	vivado_hls -f tcl/csynth_r_ev.tcl

wrapper:
	cd tools/HLS_Parser-1.0.0/src && rustc main.rs
	cd tools/HLS_Wrapper && $(MAKE) install
	tools/HLS_Parser-1.0.0/src/main eHLS_Vivado_project/solution1/syn/verilog/sp.v > tools/HLS_Wrapper/inputs/parser.out
	cd tools/HLS_Wrapper && $(MAKE) run
	cp tools/HLS_Wrapper/outputs/wrapper.sv wrp_prj/wrp_dir

zip:
	echo Zip\'d filename: $(ZIPFILE)
	zip $(ZIPFILE) -r $(ZFILES)

# wrp* from Andrew
wrp_synth:
	cd wrp_prj && vivado -mode tcl -source vivado_proj.tcl

wrp_clean:
	cd wrp_prj; rm -r vivado.jou vivado.log v_wrp_prj

# Outdated:
debug_e:
	$(CC) $(FILES_E) $(OPT_E) -D _LOCAL_AP_INT_H_
gold_e:
	$(CC) $(FILES_E) $(OPT_E) -D GOLD -D _LOCAL_AP_INT_H_
debug:
	$(CC) $(FILES) $(OPT) -D _LOCAL_AP_INT_H_
gold:
	$(CC) $(FILES) $(OPT) -D GOLD -D _LOCAL_AP_INT_H_

