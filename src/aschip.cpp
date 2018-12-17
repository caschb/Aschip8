#include <cstdint>
#include <ctime>
#include <cstdlib>

#include "aschip8.h"
#include "display.h"

AsChip8::AsChip8() : I(0), delay_timer(0), sound_timer(0), program_counter(0x200), stack_pointer(0), display(screen_mem){
	for(uint8_t i = 0; i < REGISTERS; ++i){
		V[i] = 0;	
	}
	for(uint8_t i = program_counter; i < MEM_SIZE; ++i){
		memory[i] = 0;
	}
	for(uint8_t i = 0; i < SCREEN_SIZE; ++i){
		screen_mem[i] = 0;
	}
	for(uint8_t i = 0; i < STACK_SIZE; ++i){
		stack[i] = 0;
	}
	for(uint8_t i = 0; i < BUTTON_NUM; ++i){
		pressed[i] = 0;
	}
	srand(time(NULL));
}

void AsChip8::cls(){
	for(uint8_t i = 0; i < SCREEN_SIZE; ++i){
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
		++program_counter;
}

void AsChip8::skip_not_equal(uint8_t x, uint8_t imm){
	if(V[x] != imm)
		++program_counter;
}

void AsChip8::skip_equal_V(uint8_t x, uint8_t y){
	if(V[x] == V[y])
		++program_counter;
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
		++program_counter;
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

void AsChip8::draw(uint8_t, x, uint8_t y, uint8_t n){
	V[F] = 0;
	uint8_t x_pos;
	uint8_t y_pos;
	for(uint8_t i = 0; i < n; ++i){
		for(uint8_t j = 0; j < 8; ++j){
			x_pos = (x + i) % SCREEN_ROWS;
			y_pos = ((y + j) * SCREEN_ROWS) % SCREEN_COLS;
			if(screen_mem[x_pos + y_pos] == 1 && (screen_mem[x_pos + y_pos] ^= memory[I+i]) == 0){
				V[F] = 1;
			}
		}
	}
}

void AsChip8::skip_pressed(uint8_t x){
	if(pressed[V[x]] == 1)
		++program_counter;
}

void AsChip8::skip_not_pressed(uint8_t x){
	if(pressed[V[x]] == 0)
		++program_counter;
}
