#include "CPU.h"

instruction::instruction(bitset<32> fetch){
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

CPU::CPU(){
	PC = 0; // Set PC to 0
  controller = 0; // Initialize controller
  ALUOp = 0;

  for (int i = 0; i < 32; i++){ // Initialize registers
		regFile[i] = 0;
	}
	for (int i = 0; i < 4096; i++){ //copy instrMEM
		dmemory[i] = (0);
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem){
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	PC += 4; //increment PC
	return instr;
}


bool CPU::Decode(instruction* curr){
  bool debugInfo = false;

  bitset<7> opcode; // Decode opcode
  for (int i = 0; i < 7; i++)
    opcode[i] = curr->instr[i]; // Read bits 0-7

  if (debugInfo)
    cout << "Opcode: " << opcode;

  if (opcode == 0) // End
    return false;
  else{
    // For debugging only
    bitset<5> rd; // Decode rd
    bitset<5> rs1; // Decode rs1
    bitset<12> imm; // Decode imm[11:0]
    // For debugging only
    if (debugInfo){
      for (int i = 0; i < 5; i++)
        rd[i] = curr->instr[i + 7]; // Read bits 7-11
      
      for (int i = 0; i < 5; i++)
        rs1[i] = curr->instr[i + 15]; // Read bits 15-19
    }

    if (opcode == 0b0000011){ // lw instruction
      controller = 0b110101;
      ALUOp = 0b0010;

      if (debugInfo){
        cout << " (lw):\t";
        for (int i = 0; i < 12; i++)
          imm[i] = curr->instr[i + 20]; // Read bits 20-31
        cout << "rd: " << rd.to_ulong() << ";\t";
        cout << "rs1: " << rs1.to_ulong() << ";\t";
        cout << "offset: " << imm.to_ulong() << endl;
      }
    }
    else if (opcode == 0b0100011){ // sw instruction
      controller = 0b010010;
      ALUOp = 0b0010;

      if (debugInfo){
        cout << " (sw):\t";
        bitset<5> rs2; // Decode rs2
        for (int i = 0; i < 5; i++)
          rs2[i] = curr->instr[i + 20]; // Read bits 20-24
        cout << "rs1: " << rs1.to_ulong() << ";\t";
        cout << "rs2: " << rs2.to_ulong() << ";\t";
        cout << "offset: " << rd.to_ulong() << endl;
      }
    }
    else{
      bitset<3> funct3; // Decode funct3
      for (int i = 0; i < 3; i++)
        funct3[i] = curr->instr[i + 12]; // Read bits 12-14

      if (opcode == 0b0010011){ // i-type instruction
        controller = 0b110000;

        if (funct3 == 0){ // addi
          ALUOp = 0b0010;

          if (debugInfo)
            cout << " (addi):\t";
        }
        else if (funct3 == 0b111){ // andi
          ALUOp = 0b0000;

          if (debugInfo)
            cout << " (andi):\t";
        }

        if (debugInfo){
          for (int i = 0; i < 12; i++)
            imm[i] = curr->instr[i + 20]; // Read bits 20-31
          cout << "rd: " << rd.to_ulong() << ";\t";
          cout << "rs1: " << rs1.to_ulong() << ";\t";
          cout << "imm: " << imm.to_ulong() << endl;
        }

      }
      else if (opcode == 0b0110011){ // r-type instruction
        controller = 0b100000;
        if (funct3 == 0b100){ // xor
          ALUOp = 0b0001;

          if (debugInfo)
            cout << " (xor):\t";
        }
        else if (funct3 == 0b101){ // sra
          ALUOp = 0b1000;

          if (debugInfo)
            cout << " (sra):\t";
        }
        else{
          bitset<7> funct7; // Decode funct7
          for (int i = 0; i < 7; i++)
            funct7[i] = curr->instr[i + 25]; // Read bits 25-31
          if (funct7 == 0){ // add
            ALUOp = 0b0010;

            if (debugInfo)
              cout << " (add):\t";
            
          }
          else if (funct7 == 0b0100000){ // sub
            if (debugInfo)
              cout << " (sub):\t";
            ALUOp = 0b0110;
          }
        }

        if (debugInfo){
          bitset<5> rs2; // Decode rs2
          for (int i = 0; i < 5; i++)
            rs2[i] = curr->instr[i + 20]; // Read bits 20-24

          cout << "rd: " << rd.to_ulong() << ";\t";
          cout << "rs1: " << rs1.to_ulong() << ";\t";
          cout << "rs2: " << rs2.to_ulong() << endl;
        }
      }
    }
  }
  return true;
}

unsigned long CPU::readPC(){
	return PC;
}

int CPU::readReg(int reg){
  return regFile[reg];
}

int CPU::readWord(int address){
  return dmemory[address];
}

void CPU::writeWord(int address, int value){
  dmemory[address] = value;
}

bitset<6> CPU::readControl(){
  return controller;
}

unsigned long CPU::read_rs1(instruction* curr){
  bitset<5> rs1; // Decode rs1
  for (int i = 0; i < 5; i++)
    rs1[i] = curr->instr[i + 15]; // Read bits 15-19
  return regFile[rs1.to_ulong()];
}

unsigned long CPU::read_rs2(instruction* curr){
  bitset<5> rs2; // Decode rs2
  for (int i = 0; i < 5; i++)
    rs2[i] = curr->instr[i + 20]; // Read bits 20-24
  return regFile[rs2.to_ulong()];
}

unsigned long CPU::read_imm(instruction* curr){
  bitset<12> imm; // Decode rs2
  for (int i = 0; i < 12; i++)
    imm[i] = curr->instr[i + 20]; // Read bits 20-31
  return imm.to_ulong();
}

unsigned long CPU::read_swOffset(instruction* curr){
  bitset<5> offset; // Decode rd
  for (int i = 0; i < 5; i++)
    offset[i] = curr->instr[i + 7]; // Read bits 7-11
  return offset.to_ulong();
}

void CPU::store(instruction* curr, int ALUResult){
  bitset<5> rd; // Decode rd
  for (int i = 0; i < 5; i++)
    rd[i] = curr->instr[i + 7]; // Read bits 7-11
  regFile[rd.to_ulong()] = ALUResult; // Store result in regFile
}

int CPU::ALUOperation(int operand1, int operand2){
  return myALU.operation(operand1, operand2, ALUOp);
}