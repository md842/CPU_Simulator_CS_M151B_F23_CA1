#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "ALU.h"

using namespace std;

class instruction {
public:
	bitset<32> instr; //instruction
	instruction(bitset<32> fetch); // constructor
};

class CPU {
private:
  int regFile[32]; // RegFile
  ALU myALU; // ALU
	int dmemory[4096]; // Data memory, byte addressable in little endian fashion
	unsigned long PC; // PC
  bitset<6> controller; // Controller
  bitset<4> ALUOp;

public:
	CPU();
	unsigned long readPC();
  int readReg(int reg);
  int readWord(int address);
  void writeWord(int address, int value);
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction* instr);
  bitset<6> readControl();
  unsigned long read_rs1(instruction* instr);
  unsigned long read_rs2(instruction* instr);
  unsigned long read_imm(instruction* instr);
  unsigned long read_swOffset(instruction* instr);
  int ALUOperation(int operand1, int operand2);
  void store(instruction* instr, int ALUResult);
};

// add other functions and objects here
