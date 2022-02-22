# HLS\_Parser
## About
A parser that reads Vivado/Vitis HLS synthesized top-level verilog files and outputs data on there inputs and outputs, which can be passed to the `HLS_Wrapper` to generate a *(currently partial)* wrapper.
## How to build from source
Running:
```
cargo build --release
```
from the root of the cloned repository, or
```
rustc main.rs
```
from the `src` folder will compile the program.
## How to use the program
This program takes a filepath *(either relative or absolute)* to a top level verilog file. (i.e.):
```
./hls_parser verilog_tl.v
```
or
```
./hls_parser /home/user/somefolder/verilog_tl.v
```
The program will return the parsed io data as text to the terminal, which can be easily redirected to a file in `bash`. (i.e.):
```
./hls_parser verilog_tl.v > parser.out
```
