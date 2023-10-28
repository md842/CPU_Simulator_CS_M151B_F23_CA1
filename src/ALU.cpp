#include "ALU.h"

int ALU::operation(int operand1, int operand2, bitset<4> ALUOp){
  int ALUResult = 0;
	if (ALUOp[3]){
    // sra instruction
    ALUResult = operand1 >> operand2;
  }
  else if (ALUOp[0]){
    // xor instruction
    ALUResult = operand1 ^ operand2;
  }
  else if (ALUOp[2]){
    // sub instruction
    ALUResult = operand1 - operand2;
  }
  else if (ALUOp[1]){
    // add/addi instruction
    ALUResult = operand1 + operand2;
  }
  else{
    // and/andi instruction
    ALUResult = operand1 & operand2;
  }
  return ALUResult;
}