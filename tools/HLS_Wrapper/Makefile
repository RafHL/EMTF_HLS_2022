CC   = g++
SRC  = src/main.cpp
OPT  = -std=c++17 -o
BIN  = bin/hls_wrapper

# Input, Template, and Output file specifications
INPT = inputs/parser.out
TMPL = template/template.sv
OUTP = outputs/wrapper.sv

install:
	$(CC) $(SRC) $(OPT) $(BIN)

run:
	$(BIN) $(INPT) $(TMPL) $(OUTP)

