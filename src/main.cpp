#include "aschip8.h"

int main(int argc, char *argv[]){
	AsChip8 aschip8(argv[1]);
uint16_t instruction;
	while(true){
		instruction = aschip8.memory[aschip8.program_counter] << 8 | aschip8.memory[aschip8.program_counter+1];
		aschip8.program_counter+=2;
		printf("PC: 0x%04x\tIR: 0x%04x\n", aschip8.program_counter, instruction);
		aschip8.decode_and_execute(instruction);
		aschip8.print_state();
		aschip8.display.draw();
	}
	return 0;
}

