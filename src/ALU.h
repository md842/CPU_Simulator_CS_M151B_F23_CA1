#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class ALU {
public:
	int operation(int operand1, int operand2, bitset<4> ALUOp);
};