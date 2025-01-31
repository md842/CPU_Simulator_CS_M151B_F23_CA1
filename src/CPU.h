#include <iostream>
#include <bitset>

#include "ALU.h"


class instruction {
public:
	std::bitset<32> instr; // instruction
	instruction(std::bitset<32> fetch); // constructor
};

class CPU {
private:
  int regFile[32]; // RegFile
  ALU myALU; // ALU
	char dmemory[4096]; // Data memory, byte addressable in little endian fashion
	unsigned long PC; // PC
  std::bitset<6> controller; // Controller
  std::bitset<4> ALUOp;

public:
	CPU();
	unsigned long readPC();
  void writePC(int override);
  void incrementPC(int amount);
  int readReg(int reg);
  int readWord(int address);
  void writeWord(int address, int value);
	std::bitset<32> Fetch(std::bitset<8> *instmem);
	bool Decode(instruction* instr);
  std::bitset<6> readControl();
  unsigned long read_rs1(instruction* instr);
  unsigned long read_rs2(instruction* instr);
  unsigned long read_imm(instruction* instr);
  unsigned long read_branchImm(instruction* instr);
  unsigned long read_swOffset(instruction* instr);
  int ALUOperation(int operand1, int operand2);
  int ALU_LT(int ALUResult);
  void store(instruction* instr, int ALUResult);
};