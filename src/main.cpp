#include "aschip8.h"
#include <unistd.h>
#include <ctime>

int main(int argc, char *argv[]){
	AsChip8 aschip8(argv[1]);
	uint16_t instruction;
	time_t start = time(NULL);
	time_t end;
	bool exit = false;
	while(!exit){
		instruction = aschip8.memory[aschip8.program_counter] << 8 | aschip8.memory[aschip8.program_counter+1];
		//printf("PC: 0x%04x Op: 0x%04x\n", aschip8.program_counter, instruction);
		aschip8.program_counter+=2;
		aschip8.decode_and_execute(instruction);
		//aschip8.print_state();
		exit = aschip8.key_handler.handle_events();
		if(aschip8.update_screen){
			aschip8.display.draw();
			aschip8.update_screen = false;
		}
		end = time(NULL);
		if(difftime(end, start) >= 1){
			aschip8.update_timers();
			start = time(NULL);
		}
	}
	aschip8.key_handler.clear();
	return 0;
}
