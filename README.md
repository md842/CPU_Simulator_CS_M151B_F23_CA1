# RISC-V CPU Simulator
This repository contains a C++ implementation of a simulator for a 32-bit single cycle RISC-V processor that implements 10 instructions: ADD, SUB, ADDI, XOR, ANDI, SRA, LW, SW, BLT, and JALR.

The implementation follows the datapath depicted below, starting at PC:

![Datapath diagram](/src/Datapath.jpg)

This is a modified version of Assignment 1 for the Fall 2023 session of UCLA CS M151B.

## Running
Navigate to the directory containing the extracted implementation, then simply run `make` with the included Makefile.
```
cd RISC-V_CPU_Simulator/src
make
```

The exact commands run by `make` are as follows:

```
g++ -Wall -O2 -pipe -fno-plt -fPIC *.cpp -o cpusim
```

The program takes one argument, which is a .txt file containing machine code in hexadecimal. The 4-byte instructions are stored in little-endian format. Example input files as well as their respective assembly programs are provided in the /trace folder.

The program outputs the contents of the CPU's a0 (x10) register and a1 (x11) registers.

Example run:

```
./cpusim ../trace/test_r.txt
(-8,23)
```

## Cleaning up
Navigate to the directory containing the extracted implementation, then simply run `make clean` with the included Makefile.

```
cd RISC-V_CPU_Simulator/src
make clean
```

The exact commands run by make clean are as follows:

```
rm -f cpusim
```
