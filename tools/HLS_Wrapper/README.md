# HLS\_Wrapper
## Compiling
To compile a binary, run:
```
make install
```

## Usage
Use
```
make run
```
to run the program. The input, template, and output file locations/names are `INPT`, `TMPL`, & `OUTP`, respectively.

The input file is expected to be to output of the `HLS_Parser`.

Edit the `template.sv` file in the `template` folder to modify the name of the wrapper module as well as to specify the correct name of the wrapped top level file, i.e. `wrapper_tf` and `coord_delay_tl` in the given file.