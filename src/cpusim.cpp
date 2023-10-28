#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[]){
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	bitset<8> instMem[4096];


	if (argc < 2) { // No file name entered
		return -1;
	}

	ifstream infile(argv[1]); // Open file specified in args
	if (!(infile.is_open() && infile.good())) {
		cout << "Error opening file!" << endl;
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
    infile >> line;
    stringstream line2(line);
    int x; 
    line2 >> x;
    instMem[i] = bitset<8>(x);
    i++;
  }
	int maxPC= i; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU;  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 
	bitset<32> curr;
	instruction instr = instruction(curr);
	bool done = true;
  int clockCycles = 0;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
    clockCycles++;
		// Fetch
		curr = myCPU.Fetch(instMem); // fetching the instruction
		instr = instruction(curr);

		// Decode
		done = myCPU.Decode(&instr); // True if valid instruction, false if opcode 0
		if (done == false) // break from loop so stats are not mistakenly updated
			break;
      
		// Operands
    bitset<6> controlSignals = myCPU.readControl();
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
      if (controlSignals[4]){ // AluSrc = 1
        // jalr instruction
        operand2 = myCPU.read_imm(&instr);
      }
      else{ // AluSrc = 0
        // blt instruction
        operand2 = myCPU.read_rs2(&instr);
      }
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
        if (controlSignals[4]){ // AluSrc = 1
          // i-type instruction
          operand2 = myCPU.read_imm(&instr);
        }
        else{ // AluSrc = 0
          // r-type instruction
          operand2 = myCPU.read_rs2(&instr);
        }
        // End of mux on ALU input 2
      }
    }
    
    else{ // RegWrite = 0
      // sw instruction
      operand2 = myCPU.read_swOffset(&instr);
    }

    // ALU
    int ALUResult = myCPU.ALUOperation(operand1, operand2);
    int toStore = ALUResult;
    if (ALUDmemMux){ // 1: Use ALU result as data memory address, store read data to RegFile
      toStore = myCPU.readWord(ALUResult);
    }

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
      if (controlSignals[4]){ // AluSrc = 1
        // jalr instruction
        myCPU.writePC(ALUResult);
      }
      else{
        // blt instruction
        if (LT) // If LT output on ALU = 1
          myCPU.incrementPC(myCPU.read_branchImm(&instr));
      }
    }
    // End of mux on input of PC
		
		// Sanity Check
		if (myCPU.readPC() > maxPC)
			break;
	}
	int a0 = myCPU.readReg(10); // Read a0 register
	int a1 = myCPU.readReg(11); // Read a1 register
  cerr << endl << "Output: ";
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	  cout << "(" << a0 << "," << a1 << ")" << endl;
	cerr << "Total clock cycles: " << clockCycles << endl;
	return 0;
}