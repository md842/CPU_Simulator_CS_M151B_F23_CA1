#include <iomanip>

#include "CPU.h"

unsigned long decode_rs1(instruction* curr);
unsigned long decode_rs2(instruction* curr);


instruction::instruction(std::bitset<32> fetch){
	instr = fetch;
}


CPU::CPU(){
	PC = 0; // Set PC to 0
  controller = 0; // Initialize controller
  ALUOp = 0;

  for (int i = 0; i < 32; i++){ // Initialize registers
		regFile[i] = 0;
	}
	for (int i = 0; i < 4096; i++){ // Copy instMem
		dmemory[i] = (0);
	}
}


std::bitset<32> CPU::Fetch(std::bitset<8> *instmem){
  // Fetch 32-bit instruction
	std::bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) +
                            ((instmem[PC + 2].to_ulong()) << 16) +
                            ((instmem[PC + 1].to_ulong()) << 8) +
                             (instmem[PC + 0].to_ulong()));
	PC += 4; // Increment PC
	return instr;
}


bool CPU::Decode(instruction* curr){
  std::bitset<7> opcode; // Decode opcode
  for (int i = 0; i < 7; i++)
    opcode[i] = curr->instr[i]; // Read bits 0-7

  std::cerr << "PC: " << std::left << std::setw(6) << PC;
  std::cerr << "Instruction: " << std::left << std::setw(8);

  if (opcode == 0){ // End of program
    std::cerr << "(nop)" << std::endl;
    return false;
  }
  else{
    std::bitset<5> rd; // Decode rd
    for (int i = 0; i < 5; i++)
      rd[i] = curr->instr[i + 7]; // Read bits 7-11

    if (opcode[6]){ // blt or jalr
      if (opcode[2]){ // jalr
        controller = 0b011000; // AluSrc, Branch
        ALUOp = 0b0010; // add

        std::cerr << "jalr";
        std::cerr << "rd:  " << std::right << std::setw(3) << 'x' + std::to_string(rd.to_ulong()) << "  ";
        std::cerr << "rs1:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs1(curr)) << "  ";
        std::cerr << "offset: " << std::right << std::setw(4) << read_imm(curr) << std::endl;
      }
      else{ // blt
        controller = 0b001000; // Branch
        ALUOp = 0b0110; // subtract

        std::cerr << "blt";
        std::cerr << "rs1: " << std::right << std::setw(3) << 'x' + std::to_string(decode_rs1(curr)) << "  ";
        std::cerr << "rs2:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs2(curr)) << "  ";
        std::cerr << "imm:    " << std::right << std::setw(4) << read_branchImm(curr) << std::endl;
      }
    }
    else if (opcode == 0b0000011){ // lw instruction
      controller = 0b110101;
      ALUOp = 0b0010;

      std::cerr << "lw";
      std::cerr << "rd:  " << std::right << std::setw(3) << 'x' + std::to_string(rd.to_ulong()) << "  ";
      std::cerr << "offset: " << std::right << std::setw(4) << read_imm(curr) << "  ";
      std::cerr << "rs1:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs1(curr)) << std::endl;
    }
    else if (opcode == 0b0100011){ // sw instruction
      controller = 0b010010;
      ALUOp = 0b0010;

      std::cerr << "sw";
      std::cerr << "rs2: " << std::right << std::setw(3) << 'x' + std::to_string(decode_rs2(curr)) << "  ";
      std::cerr << "offset: " << std::right << std::setw(4) << rd.to_ulong() << "  ";
      std::cerr << "rs1:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs1(curr)) << std::endl;
    }
    else{
      std::bitset<3> funct3; // Decode funct3
      for (int i = 0; i < 3; i++)
        funct3[i] = curr->instr[i + 12]; // Read bits 12-14

      if (opcode == 0b0010011){ // i-type instruction
        controller = 0b110000;

        if (funct3 == 0){ // addi
          ALUOp = 0b0010;
          std::cerr << "addi"; // Debug info
        }
        else if (funct3 == 0b111){ // andi
          ALUOp = 0b0000;
          std::cerr << "andi"; // Debug info
        }

        std::cerr << "rd:  " << std::right << std::setw(3) << 'x' + std::to_string(rd.to_ulong()) << "  ";
        std::cerr << "rs1:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs1(curr)) << "  ";
        std::cerr << "imm:    " << std::right << std::setw(4) << read_imm(curr) << std::endl;
      }
      else if (opcode == 0b0110011){ // r-type instruction
        controller = 0b100000;
        if (funct3 == 0b100){ // xor
          ALUOp = 0b0001;
          std::cerr << "xor"; // Debug info
        }
        else if (funct3 == 0b101){ // sra
          ALUOp = 0b1000;
          std::cerr << "sra"; // Debug info
        }
        else{
          std::bitset<7> funct7; // Decode funct7
          for (int i = 0; i < 7; i++)
            funct7[i] = curr->instr[i + 25]; // Read bits 25-31
          if (funct7 == 0){ // add
            ALUOp = 0b0010;
            std::cerr << "add"; // Debug info
          }
          else if (funct7 == 0b0100000){ // sub
            ALUOp = 0b0110;
            std::cerr << "sub"; // Debug info
          }
        }

        std::cerr << "rd:  " << std::right << std::setw(3) << 'x' + std::to_string(rd.to_ulong()) << "  ";
        std::cerr << "rs1:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs1(curr)) << "  ";
        std::cerr << "rs2:    " << std::right << std::setw(4) << 'x' + std::to_string(decode_rs2(curr)) << std::endl;
      }
    }
  }
  return true;
}


unsigned long CPU::readPC(){
	return PC;
}


void CPU::writePC(int override){
  PC = override;
}


void CPU::incrementPC(int amount){
  PC = PC + amount - 4; // Adjust by -4 because next fetch will +4
}


int CPU::readReg(int reg){
  return regFile[reg];
}


int CPU::readWord(int address){
  return dmemory[address];
}


void CPU::writeWord(int address, int value){
  dmemory[address] = value;
  dmemory[address + 1] = value >> 8;
  dmemory[address + 2] = value >> 16;
  dmemory[address + 3] = value >> 24;
}


std::bitset<6> CPU::readControl(){
  return controller;
}


/// Non-member helper function that decodes rs1 specified by instruction curr.
unsigned long decode_rs1(instruction* curr){
  std::bitset<5> rs1; // Decode rs1
  for (int i = 0; i < 5; i++)
    rs1[i] = curr->instr[i + 15]; // Read bits 15-19
  return rs1.to_ulong();
}


unsigned long CPU::read_rs1(instruction* curr){
  return regFile[decode_rs1(curr)];
}


/// Non-member helper function that decodes rs2 specified by instruction curr.
unsigned long decode_rs2(instruction* curr){
  std::bitset<5> rs2; // Decode rs2
  for (int i = 0; i < 5; i++)
    rs2[i] = curr->instr[i + 20]; // Read bits 20-24
  return rs2.to_ulong();
}


unsigned long CPU::read_rs2(instruction* curr){
  return regFile[decode_rs2(curr)];
}


unsigned long CPU::read_imm(instruction* curr){
  std::bitset<12> imm; // Decode imm[11:0]
  for (int i = 0; i < 12; i++)
    imm[i] = curr->instr[i + 20]; // Read bits 20-31
  int result = imm.to_ulong();

  if (result > 2048) // Result is negative
    result = -4096 + result;

  return result;
}


unsigned long CPU::read_branchImm(instruction* curr){
  std::bitset<12> imm; // Decode imm[12|10:5] | imm[4:1|11]
  
  // Using [11:0] to represent [12:1] and multiplying by 2 at the end
  imm[11] = curr->instr[31]; // Read bit 31
  for (int i = 4; i < 10; i++)
    imm[i] = curr->instr[i + 21]; // Read bits 25-30
  imm[10] = curr->instr[7]; // Read bit 7
  for (int i = 0; i < 4; i++)
    imm[i] = curr->instr[i + 8]; // Read bits 8-12
  std::bitset<5> rs2; // Decode rs2
  for (int i = 0; i < 5; i++)
    rs2[i] = curr->instr[i + 20]; // Read bits 20-24
  int result = imm.to_ulong() * 2;

  if (result > 4096) // Result is negative
    result = -8192 + result;

  return result;
}


unsigned long CPU::read_swOffset(instruction* curr){
  std::bitset<5> offset; // Decode offset
  for (int i = 0; i < 5; i++)
    offset[i] = curr->instr[i + 7]; // Read bits 7-11
  return offset.to_ulong();
}


void CPU::store(instruction* curr, int ALUResult){
  std::bitset<5> rd; // Decode rd
  for (int i = 0; i < 5; i++)
    rd[i] = curr->instr[i + 7]; // Read bits 7-11
  regFile[rd.to_ulong()] = ALUResult; // Store result in regFile
}


int CPU::ALUOperation(int operand1, int operand2){
  return myALU.operation(operand1, operand2, ALUOp);
}


int CPU::ALU_LT(int ALUResult){
  return myALU.LT(ALUResult);
}