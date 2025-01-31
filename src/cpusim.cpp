#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>

#include "CPU.h"


int main(int argc, char* argv[]){
	// Read the program (specified in args) into instruction memory.

  // 4KB instruction memory; each instruction is 32 bits (little-endian)
	std::bitset<8> instMem[4096]; // Each cell represents one byte

	if (argc < 2) // No program specified
		return -1;

	std::ifstream inFile(argv[1]); // Open program specified in args
	if (!(inFile.is_open() && inFile.good())){
		std::cout << "Error opening file!" << std::endl;
		return -1;
	}

	std::string token;
  std::string hexInput;
  unsigned short convertedByte;

  /* Read entire program from file token by token. The advantage of this
     approach is that spaces and/or newlines in the input do not matter. */
  while (inFile){
    inFile >> token;
    hexInput += token;
  }

  // Parse entire program, two hex digits (one byte) at a time
	for (unsigned short i = 0; i * 2 < hexInput.length(); i++){
    std::string byte = hexInput.substr(i * 2, 2); // Get current hex byte

    // Convert hex byte to unsigned short
    std::istringstream convert(byte);
    convert >> std::hex >> convertedByte;

    // Convert unsigned short to binary and store in instruction memory
    instMem[i] = std::bitset<8>(convertedByte);
  }
	unsigned short maxPC = hexInput.length() / 2; 

	/* The CPU class defines different components of the processor and has
     functions for each stage of the execution pipeline (e.g., fetching an
     instruction, decoding, etc.). */
	CPU myCPU;

  int clockCycles = 0;
	while (true){ // CPU's main execution loop
    clockCycles++; // Each iteration is one clock cycle

		// Fetch stage
		std::bitset<32> curr = myCPU.Fetch(instMem); // Fetch the instruction
		instruction instr = instruction(curr);

		// Decode stage
		if (!myCPU.Decode(&instr)) // True if valid instruction, false if opcode 0
			break;
      
		// Decode operands and destination from the instruction and control signals
    std::bitset<6> controlSignals = myCPU.readControl();
    // Bit 5: RegWrite
    // Bit 4: AluSrc
    // Bit 3: Branch
    // Bit 2: MemRe
    // Bit 1: MemWr
    // Bit 0: MemtoReg

    unsigned long operand1 = myCPU.read_rs1(&instr); // ALU input 1
    unsigned long operand2 = 0; // ALU input 2
    int ALUDmemMux = 0; // This variable acts as the mux on the outputs of ALU and data memory
    // 0: Store ALU result to RegFile
    // 1: Use ALU result as data memory address, store read data to RegFile

    // The following if statement acts as the AND gate with Branch control signal
    if (controlSignals[3]){ // Branch = 1
      // This if/else block acts as the mux on ALU input 2
      if (controlSignals[4]) // AluSrc = 1
        // jalr instruction
        operand2 = myCPU.read_imm(&instr);
      else // AluSrc = 0
        // blt instruction
        operand2 = myCPU.read_rs2(&instr);
      // End of mux on ALU input 2
    }

    else if (controlSignals[5]){ // RegWrite = 1
      if (controlSignals[2]){ // MemRe = 1
        // lw instruction
        ALUDmemMux = 1; // 1: Use ALU result as data memory address, store read data to RegFile
        operand2 = myCPU.read_imm(&instr);
      }
      else{ // MemRe = 0
        // This if/else block acts as the mux on ALU input 2
        if (controlSignals[4]) // AluSrc = 1
          // i-type instruction
          operand2 = myCPU.read_imm(&instr);
        else // AluSrc = 0
          // r-type instruction
          operand2 = myCPU.read_rs2(&instr);
        // End of mux on ALU input 2
      }
    }
    
    else // RegWrite = 0
      // sw instruction
      operand2 = myCPU.read_swOffset(&instr);

    // Perform ALU operation
    int ALUResult = myCPU.ALUOperation(operand1, operand2);
    int toStore = ALUResult;
    if (ALUDmemMux) // 1: Use ALU result as data memory address, store read data to RegFile
      toStore = myCPU.readWord(ALUResult);

    // Store results
    bool LT = true;
    if (controlSignals[3]){ // Branch = 1
      if (controlSignals[4]) // AluSrc = 1
        // jalr instruction
        toStore = myCPU.readPC();
      else
        // blt instruction
        LT = myCPU.ALU_LT(ALUResult);
    }

    if (controlSignals[1]) // RegWr = 1; sw instruction
      myCPU.writeWord(ALUResult, myCPU.read_rs2(&instr));
    else // RegWr = 0 
      // (r-type, i-type, lw) || (jalr)
      if ((controlSignals[3] == 0) || (controlSignals[3] && controlSignals[4]))
        myCPU.store(&instr, toStore);

    // Update PC
    // This if statement acts as the mux on the input of PC; the other input (PC + 4 was defined within CPU::Fetch())
    if (controlSignals[3]){ // Branch = 1
      if (controlSignals[4]) // AluSrc = 1
        // jalr instruction
        myCPU.writePC(ALUResult);
      else{
        // blt instruction
        if (LT) // If LT output on ALU = 1
          myCPU.incrementPC(myCPU.read_branchImm(&instr));
      }
    }
    // End of mux on input of PC
		
		if (myCPU.readPC() > maxPC) // Sanity check
			break;
	}

  std::cout << "Register a0 (x10): " << myCPU.readReg(10) << std::endl;
  std::cout << "Register a1 (x11): " << myCPU.readReg(11) << std::endl;
	std::cout << "Total clock cycles: " << clockCycles << std::endl;
	return 0;
}