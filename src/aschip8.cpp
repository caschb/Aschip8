#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#include "aschip8.h"
#include "display.h"

AsChip8::AsChip8(const char * filename) : I(0), delay_timer(0), sound_timer(0), program_counter(0x200), stack_pointer(0), display(&screen_mem[0]){
	for(uint8_t i = 0; i < REGISTERS; ++i){
		V[i] = 0;	
	}
	for(uint16_t i = program_counter; i < MEM_SIZE; ++i){
		memory[i] = 0;
	}
	for(uint16_t i = 0; i < SCREEN_SIZE; ++i){
		screen_mem[i] = 0;
	}
	for(uint8_t i = 0; i < STACK_SIZE; ++i){
		stack[i] = 0;
	}
	for(uint8_t i = 0; i < BUTTON_NUM; ++i){
		pressed[i] = 0;
	}
	srand(time(NULL));
	FILE * program = fopen(filename, "rb");
	int i = 0;
	while((fread(&memory[program_counter + i++], 1, 1, program))){
	}
}

void AsChip8::cls(){
	for(uint16_t i = 0; i < SCREEN_SIZE; ++i){
		screen_mem[i] = 0;
	}
}

void AsChip8::ret(){
	program_counter = stack[stack_pointer--];
}

void AsChip8::jp(uint16_t mem_location){
	program_counter = mem_location;	
}

void AsChip8::call(uint16_t mem_location){
	stack[++stack_pointer] = program_counter;
	program_counter = mem_location;
}

void AsChip8::skip_equal(uint8_t x, uint8_t imm){
	if(V[x] == imm)
		program_counter+=2;
}

void AsChip8::skip_not_equal(uint8_t x, uint8_t imm){
	if(V[x] != imm)
		program_counter+=2;
}

void AsChip8::skip_equal_V(uint8_t x, uint8_t y){
	if(V[x] == V[y])
		program_counter+=2;
}

void AsChip8::load(uint8_t x, uint8_t imm){
	V[x] = imm;
}

void AsChip8::add(uint8_t x, uint8_t imm){
	V[x] += imm;
}

void AsChip8::load_V(uint8_t x, uint8_t y){
	V[x] = V[y];
}

void AsChip8::or_V(uint8_t x, uint8_t y){
	V[x] |= V[y];
}

void AsChip8::and_V(uint8_t x, uint8_t y){
	V[x] &= V[y];
}

void AsChip8::xor_V(uint8_t x, uint8_t y){
	V[x] ^= V[y];
}

void AsChip8::add_V(uint8_t x, uint8_t y){
	V[x] += V[y];
}

void AsChip8::sub_V(uint8_t x, uint8_t y){
	V[0xF] = V[x] > V[y] ? 1 : 0;
	V[x] = V[y] - V[x];
}

void AsChip8::shift_right(uint8_t x){
	V[0xF] = V[x]%2 != 0 ? 1 : 0;
	V[x] >>= 1;
}

void AsChip8::subn_V(uint8_t x, uint8_t y){
	V[0xF] = V[y] > V[x] ? 1 : 0;
	V[x] -= V[y];
}

void AsChip8::shift_left(uint8_t x){
	V[0xF] = V[x]&0x80 == 0x80 ? 1 : 0;
	V[x] <<= 1;
}

void AsChip8::skip_not_equal_V(uint8_t x, uint8_t y){
	if(V[x] != V[y])
		program_counter+=2;
}

void AsChip8::load_I(uint16_t mem){
	I = mem;
}

void AsChip8::jump_with_offset(uint16_t offset){
	program_counter = V[0] + offset;
}

void AsChip8::random_number(uint8_t x, uint8_t imm){
		V[x] = (rand() % 256) % imm; 
}

void AsChip8::draw(uint8_t x, uint8_t y, uint8_t n){
	V[0xF] = 0;
	uint8_t byte;
	uint8_t bit;
	int x_val;
	int y_val;
	for(int i = 0; i < n; ++i){
		byte = memory[I+i];
		for(int j = 0; j < 8; ++j){
			x_val =(V[x]+(7-j)) % SCREEN_COLS;
			y_val = ((V[y] + i) % SCREEN_ROWS) * SCREEN_COLS;
			bit = (byte >> j) & 0x1;
			if(bit == 1 && screen_mem[x_val + y_val] == 1){
				V[0xF] = 1;
			}
			screen_mem[x_val + y_val] = screen_mem[x_val + y_val] ^ bit;
		}
	}
}

void AsChip8::skip_pressed(uint8_t x){
	if(pressed[V[x]] == 1)
		program_counter+=2;
}

void AsChip8::skip_not_pressed(uint8_t x){
	if(pressed[V[x]] == 0)
		program_counter+=2;
}

void AsChip8::load_V_DT(uint8_t x){
	V[x] = delay_timer;
}

void AsChip8::wait_for_key(uint8_t x){
	bool is_pressed = false;
	while(!is_pressed){
		for(uint8_t i = 0; i < BUTTON_NUM; ++i){
			if(pressed[i] == 1){
				is_pressed = true;
			}
		}
	}
}

void AsChip8::load_DT_V(uint8_t x){
	delay_timer = V[x];
}

void AsChip8::load_ST_V(uint8_t x){
	sound_timer = V[x];
}
		
void AsChip8::add_I_V(uint8_t x){
	I+=V[x];
}

void AsChip8::load_char(uint8_t x){
	I = V[x];
}

void AsChip8::load_bcd(uint8_t x){
	uint8_t hundreds = V[x]/100;
	uint8_t tens = V[x]/10 - hundreds * 10;
	uint8_t units = V[x] - hundreds * 100 - tens * 10;
	memory[I] = hundreds;
	memory[I + 1] = tens;
	memory[I + 2] = units;
}

void AsChip8::store_registers(uint8_t x){
	for(uint8_t i = 0; i <= x; ++i){
		memory[I + i] = V[i];
	}
}

void AsChip8::load_registers(uint8_t x){
	for(uint8_t i = 0; i <= x; ++i){
		V[i] = memory[I + i];
	}
}

void AsChip8::print_state(){
	for(int i = 0; i < REGISTERS; ++i){
		printf("V[0x%01x]=0x%02x\n", i, V[i]);
	}
	printf("I=0x%04x\n", I);
}

void AsChip8::decode_and_execute(uint16_t instruction){
			if(instruction == 0x00E0){
				cls();
			} else if (instruction == 0x00EE){
				ret();
			} else if ((instruction & 0xF000) == 0xF000){
				uint8_t arg	= (instruction & 0x0F00)>>8;
				if((instruction & 0xF065) == 0xF065){
					load_registers(arg);
				} else if ((instruction & 0xF055) == 0xF055){
					store_registers(arg);
				} else if ((instruction & 0xF033) == 0xF033){
					load_bcd(arg);
				} else if ((instruction & 0xF029) == 0xF029){
					load_char(arg);
				} else if ((instruction & 0xF01E) == 0xF01E){
					add_I_V(arg);
				} else if ((instruction & 0xF018) == 0xF018){
					load_ST_V(arg);
				} else if ((instruction & 0xF015) == 0xF015){
					load_DT_V(arg);
				} else if ((instruction & 0xF00A) == 0xF00A){
					wait_for_key(arg);
				} else if ((instruction & 0xF007) == 0xF007){
					load_V_DT(arg);
				} else {
					print_state();
				}
			} else if ((instruction & 0xE000) == 0xE000){
				uint8_t arg	= (instruction & 0x0F00)>>8;
				if((instruction & 0xE0A1) == 0xE0A1){
					skip_not_pressed(arg);	
				} else if ((instruction & 0xE09E) == 0xE09E){
					skip_pressed(arg);
				} else {
					print_state();
				}
			} else if ((instruction & 0xD000) == 0xD000){
				//printf("draw: 0x%04x 0x%04x 0x%04x\n",(instruction & 0x0F00)>>8, (instruction & 0x00F0)>>4, instruction & 0x000F);
				draw((instruction & 0x0F00)>>8, (instruction & 0x00F0)>>4, instruction & 0x000F);
			} else if ((instruction & 0xC000) == 0xC000){
				random_number((instruction & 0x0F00)>>8, (instruction & 0x00FF));
			} else if ((instruction & 0xB000) == 0xB000){
				jump_with_offset(instruction & 0x0FFF);
			} else if ((instruction & 0xA000) == 0xA000){
				load_I(instruction & 0x0FFF);
			} else if ((instruction & 0x9000) == 0x9000){
				skip_not_equal_V((instruction & 0x0F00)>>8, (instruction & 0x00F0)>>4);
			} else if ((instruction & 0x8000) == 0x8000){
				uint8_t arg1 = (instruction & 0x0F00)>>8;
				uint8_t arg2 = (instruction & 0x00F0)>>4;
				if((instruction & 0x000E) == 0x000E){
					shift_left(arg1);
				} else if ((instruction & 0x0007) == 0x0007){
					subn_V(arg1, arg2);
				} else if ((instruction & 0x0006) == 0x0006){
					shift_right(arg1);
				} else if ((instruction & 0x0005) == 0x0005){
					sub_V(arg1, arg2);
				} else if ((instruction & 0x0004) == 0x0004){
					add_V(arg1, arg2);
				} else if ((instruction & 0x0003) == 0x0003){
					xor_V(arg1, arg2);
				} else if ((instruction & 0x0002) == 0x0002){
					and_V(arg1, arg2);
				} else if ((instruction & 0x0001) == 0x0001){
					or_V(arg1, arg2);
				} else {
					printf("load_V 0x%02x 0x%02x\n", arg1, arg2);
					load_V(arg1, arg2);
				}
			} else if ((instruction & 0x7000) == 0x7000){
				add((instruction & 0x0F00)>>8, (instruction & 0x00FF));				
			} else if ((instruction & 0x6000) == 0x6000){
				load((instruction & 0x0F00)>>8, (instruction & 0x00FF));				
			} else if ((instruction & 0x5000) == 0x5000){
				skip_equal_V((instruction & 0x0F00)>>8, (instruction & 0x00F0)>>4);
			} else if ((instruction & 0x4000) == 0x4000){
				skip_not_equal((instruction & 0x0F00)>>8, instruction & 0x00FF);
			} else if ((instruction & 0x3000) == 0x3000){
				skip_equal((instruction & 0x0F00)>>8, instruction & 0x00FF);
			} else if ((instruction & 0x2000) == 0x2000){
				call(instruction & 0x0FFF);
			} else if ((instruction & 0x1000) == 0x1000){
				jp(instruction & 0x0FFF);
			}
}
