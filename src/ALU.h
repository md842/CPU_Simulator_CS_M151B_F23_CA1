#include <bitset>


class ALU {
public:
	int operation(int operand1, int operand2, std::bitset<4> ALUOp);
  bool LT(int ALUResult);
};