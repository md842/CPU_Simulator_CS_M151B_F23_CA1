# RISC-V CPU Simulator
This repository contains a C++ implementation of a simulator for a 32-bit single cycle RISC-V processor that implements 10 instructions: ADD, SUB, ADDI, XOR, ANDI, SRA, LW, SW, BLT, and JALR.

The implementation follows the datapath depicted below, starting at PC:

![Datapath diagram](/src/Datapath.jpg)

This is a modified version of Assignment 1 for the Fall 2023 session of UCLA CS M151B.

## Running
Navigate to the directory containing the extracted implementation, then simply run `make` with the included Makefile.
```console
$ cd RISC-V_CPU_Simulator/src
src$ make
```

The exact commands run by `make` are as follows:

```
g++ -Wall -O2 -pipe -fno-plt -fPIC *.cpp -o cpusim
```

The program takes one argument, which is a .txt file containing machine code in hexadecimal. The 4-byte instructions are stored in little-endian format. Example input files as well as their respective assembly programs are provided in the /trace folder.

The program outputs the contents of the CPU's a0 (x10) register and a1 (x11) registers.

Example run:

```console
src$ ./cpusim ../trace/test_all.txt
PC: 4     Instruction: addi    rd:   x1  rs1:      x0  imm:       1
PC: 8     Instruction: addi    rd:   x2  rs1:      x0  imm:       8
PC: 12    Instruction: sub     rd:   x3  rs1:      x2  rs2:      x2
PC: 16    Instruction: blt     rs1:  x2  rs2:      x1  imm:       8
PC: 20    Instruction: add     rd:   x2  rs1:      x1  rs2:      x0
PC: 24    Instruction: blt     rs1:  x3  rs2:      x1  imm:       8
PC: 32    Instruction: sw      rs2:  x2  offset:    0  rs1:      x0
PC: 36    Instruction: addi    rd:   x4  rs1:      x0  imm:      32
PC: 40    Instruction: jalr    rd:   x3  rs1:      x4  offset:   12
PC: 48    Instruction: sw      rs2:  x3  offset:    4  rs1:      x0
PC: 52    Instruction: lw      rd:  x10  offset:    4  rs1:      x0
PC: 56    Instruction: lw      rd:  x11  offset:    0  rs1:      x0
PC: 60    Instruction: (nop)   
Register a0 (x10): 40
Register a1 (x11): 1
Total clock cycles: 13
```

## Cleaning up
Navigate to the directory containing the extracted implementation, then simply run `make clean` with the included Makefile.

```console
$ cd RISC-V_CPU_Simulator/src
src$ make clean
```

The exact commands run by make clean are as follows:

```
rm -f cpusim
```
