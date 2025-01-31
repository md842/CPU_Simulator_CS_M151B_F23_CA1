#include "ALU.h"


int ALU::operation(int operand1, int operand2, std::bitset<4> ALUOp){
	if (ALUOp[3]) // sra instruction
    return operand1 >> operand2;
  else if (ALUOp[0]) // xor instruction
    return operand1 ^ operand2;
  else if (ALUOp[2]) // sub instruction
    return operand1 - operand2;
  else if (ALUOp[1]) // add/addi instruction
    return operand1 + operand2;
  else // and/andi instruction
    return operand1 & operand2;
}


bool ALU::LT(int ALUResult){
  if (ALUResult < 0) // reg[rs1] < reg[rs2]
    return true;
  return false;
}