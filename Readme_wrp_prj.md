# Vivado Wrapper Synthesis Script
## Use
- Run `make wrp_synth` from project root to run the synthesis
- Timing summary, resource utilization, critical path, and clock utilization reports will be generated in the `wrp_prj/reports` directory

## Note
- The script utilizes the `eHLS_Vivado_project` directory created by HLS synthesis. This script project should be compatible with the HLS projects. Integrate it by appending the contents of the `Makefile` to the HLS project's `Makefile`, and copy the `wrp_prj` directory into the root of the project (the save level that the `Makefile` is).
- This script utilizes the wrapper `wrp_prj/wrp_dir/wrapper.sv` as a top level. Modifications to the IO of `sp.v` will require the recreation of this file.