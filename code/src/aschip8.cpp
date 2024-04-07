#include <aschip8.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <display.h>
#include <key_handler.h>

AsChip8::AsChip8(const char *filename)
    : I(0), delay_timer(0), sound_timer(0), program_counter(0x200),
      stack_pointer(0), display(&screen_mem[0]), update_screen(false),
      key_handler(&pressed[0]) {
  for (uint8_t i = 0; i < REGISTERS; ++i) {
    V[i] = 0;
  }
  for (uint16_t i = program_counter; i < MEM_SIZE; ++i) {
    memory[i] = 0;
  }
  for (uint16_t i = 0; i < SCREEN_SIZE; ++i) {
    screen_mem[i] = 0;
  }
  for (uint8_t i = 0; i < STACK_SIZE; ++i) {
    stack[i] = 0;
  }
  for (uint8_t i = 0; i < BUTTON_NUM; ++i) {
    pressed[i] = 0;
  }
  srand(time(NULL));
  FILE *program = fopen(filename, "rb");
  int i = 0;
  while ((fread(&memory[program_counter + i++], 1, 1, program))) {
  }

  uint8_t font_set[] = {
      0xf0, 0x90, 0x90, 0x90, 0xf0, /*0*/
      0x20, 0x60, 0x20, 0x20, 0x70, /*1*/
      0xf0, 0x10, 0xf0, 0x80, 0xf0, /*2*/
      0xf0, 0x10, 0xf0, 0x10, 0xf0, /*3*/
      0x90, 0x90, 0xf0, 0x10, 0x10, /*4*/
      0xf0, 0x80, 0xf0, 0x10, 0xf0, /*5*/
      0xf0, 0x80, 0xf0, 0x90, 0xf0, /*6*/
      0xf0, 0x10, 0x20, 0x40, 0x40, /*7*/
      0xf0, 0x90, 0xf0, 0x90, 0xf0, /*8*/
      0xf0, 0x90, 0xf0, 0x10, 0xf0, /*9*/
      0xf0, 0x90, 0xf0, 0x90, 0x90, /*A*/
      0xe0, 0x90, 0xe0, 0x90, 0xe0, /*B*/
      0xf0, 0x80, 0x80, 0x80, 0xf0, /*C*/
      0xe0, 0x90, 0x90, 0x90, 0xe0, /*D*/
      0xf0, 0x80, 0xf0, 0x80, 0xf0, /*E*/
      0xf0, 0x80, 0xf0, 0x80, 0x80  /*F*/
  };

  for (int i = 0; i < FONT_SIZE * FONTSET_LEN; ++i) {
    memory[i] = font_set[i];
  }
}

void AsChip8::cls() {
  for (uint16_t i = 0; i < SCREEN_SIZE; ++i) {
    screen_mem[i] = 0;
  }
  update_screen = true;
}

void AsChip8::ret() { program_counter = stack[stack_pointer--]; }

void AsChip8::jp(uint16_t mem_location) { program_counter = mem_location; }

void AsChip8::call(uint16_t mem_location) {
  stack[++stack_pointer] = program_counter;
  program_counter = mem_location;
}

void AsChip8::skip_equal(uint8_t x, uint8_t imm) {
  if (V[x] == imm)
    program_counter += 2;
}

void AsChip8::skip_not_equal(uint8_t x, uint8_t imm) {
  if (V[x] != imm)
    program_counter += 2;
}

void AsChip8::skip_equal_V(uint8_t x, uint8_t y) {
  if (V[x] == V[y])
    program_counter += 2;
}

void AsChip8::load(uint8_t x, uint8_t imm) { V[x] = imm; }

void AsChip8::add(uint8_t x, uint8_t imm) { V[x] += imm; }

void AsChip8::load_V(uint8_t x, uint8_t y) { V[x] = V[y]; }

void AsChip8::or_V(uint8_t x, uint8_t y) { V[x] |= V[y]; }

void AsChip8::and_V(uint8_t x, uint8_t y) { V[x] &= V[y]; }

void AsChip8::xor_V(uint8_t x, uint8_t y) { V[x] ^= V[y]; }

void AsChip8::add_V(uint8_t x, uint8_t y) {
  V[0xF] = 0;
  int sum = (int)V[x] + (int)V[y];
  V[x] = sum & 0xFF;
  if (sum > 255) {
    V[0xF] = 1;
  }
}

void AsChip8::sub_V(uint8_t x, uint8_t y) {
  V[0xF] = V[x] > V[y] ? 1 : 0;
  V[x] = V[x] - V[y];
}

void AsChip8::shift_right(uint8_t x) {
  V[0xF] = V[x] % 2;
  V[x] /= 2;
}

void AsChip8::subn_V(uint8_t x, uint8_t y) {
  V[0xF] = V[y] > V[x] ? 1 : 0;
  V[x] = V[y] - V[x];
}

void AsChip8::shift_left(uint8_t x) {
  V[0xF] = ((V[x] & 0x80) == 0x80) ? 1 : 0;
  V[x] *= 2;
}

void AsChip8::skip_not_equal_V(uint8_t x, uint8_t y) {
  if (V[x] != V[y])
    program_counter += 2;
}

void AsChip8::load_I(uint16_t mem) { I = mem; }

void AsChip8::jump_with_offset(uint16_t offset) {
  program_counter = (uint16_t)V[0] + offset;
}

void AsChip8::random_number(uint8_t x, uint8_t imm) {
  V[x] = (rand() % 256) & imm;
}

void AsChip8::draw(uint8_t x, uint8_t y, uint8_t n) {
  update_screen = true;
  V[0xF] = 0;
  uint8_t byte;
  uint8_t bit;
  int x_val;
  int y_val;
  for (int i = 0; i < n; ++i) {
    byte = memory[I + i];
    for (int j = 0; j < 8; ++j) {
      x_val = (V[x] + (7 - j)) % SCREEN_COLS;
      y_val = ((V[y] + i) % SCREEN_ROWS) * SCREEN_COLS;
      bit = (byte >> j) & 0x1;
      if (bit == 1 && screen_mem[x_val + y_val] == 1) {
        V[0xF] = 1;
      }
      screen_mem[x_val + y_val] = screen_mem[x_val + y_val] ^ bit;
    }
  }
}

void AsChip8::skip_pressed(uint8_t x) {
  if (pressed[V[x]] == 1)
    program_counter += 2;
}

void AsChip8::skip_not_pressed(uint8_t x) {
  if (pressed[V[x]] == 0)
    program_counter += 2;
}

void AsChip8::load_V_DT(uint8_t x) { V[x] = delay_timer; }

void AsChip8::wait_for_key(uint8_t x) {
  bool is_pressed = false;
  while (!is_pressed) {
    for (uint8_t i = 0; i < BUTTON_NUM; ++i) {
      key_handler.handle_events();
      if (pressed[i] == 1) {
        V[x] = i;
        is_pressed = true;
      }
    }
  }
}

void AsChip8::load_DT_V(uint8_t x) { delay_timer = V[x]; }

void AsChip8::load_ST_V(uint8_t x) { sound_timer = V[x]; }

void AsChip8::add_I_V(uint8_t x) { I += (uint16_t)V[x]; }

void AsChip8::load_char(uint8_t x) { I = FONT_SIZE * (uint16_t)V[x]; }

void AsChip8::load_bcd(uint8_t x) {
  uint8_t hundreds = V[x] / 100;
  uint8_t tens = V[x] / 10 - hundreds * 10;
  uint8_t units = V[x] - hundreds * 100 - tens * 10;
  memory[I] = hundreds;
  memory[I + 1] = tens;
  memory[I + 2] = units;
}

void AsChip8::store_registers(uint8_t x) {
  for (uint8_t i = 0; i <= x; ++i) {
    memory[I + (uint16_t)i] = V[i];
  }
}

void AsChip8::load_registers(uint8_t x) {
  for (uint8_t i = 0; i <= x; ++i) {
    V[i] = memory[I + (uint16_t)i];
  }
}

void AsChip8::print_state() {
  for (int i = 0; i < REGISTERS; ++i) {
    printf("V[0x%01x]=0x%02x\n", i, V[i]);
  }
  printf("I=0x%04x\n", I);
  /*
  printf("DT=0x%02x\n", delay_timer);
  printf("ST=0x%02x\n", delay_timer);
  */
  printf("SP=0x%02x\n", stack_pointer);
}

bool AsChip8::update_delay_timer() {
  if (delay_timer == 0)
    return false;
  --delay_timer;
  return true;
}

void AsChip8::update_sound_timer() {
  if (sound_timer > 0) {
    printf("beep\n");
    --sound_timer;
  }
}

// I basically lifted this other way of doing the decoding from
// https://github.com/dmatlack/chip8
void AsChip8::decode_and_execute(uint16_t instruction) {
  uint8_t x = (instruction >> 8) & 0x000F;     // xFxx
  uint8_t y = (instruction >> 4) & 0x000F;     // xxFx
  uint8_t lead = (instruction >> 12) & 0x000F; // Fxxx
  uint8_t n = instruction & 0x000F;            // xxxF
  uint8_t kk = instruction & 0x00FF;           // xxFF
  uint16_t nnn = instruction & 0x0FFF;         // xFFF

  switch (lead) {
  case 0x0:
    switch (nnn) {
    case 0x0E0:
      // printf("CLS\n");
      cls();
      break;
    case 0x0EE:
      // printf("RET\n");
      ret();
      break;
      // default:
      // printf("Error! 0x%04x\n", instruction);
    }
    break;
  case 0x1:
    // printf("JUMP 0x%04x\n", nnn);
    jp(nnn);
    break;
  case 0x2:
    // printf("CALL 0x%04x\n", nnn);
    call(nnn);
    break;
  case 0x3:
    // printf("SE 0x%02x 0x%02x\n", x, kk);
    skip_equal(x, kk);
    break;
  case 0x4:
    // printf("SNE 0x%02x 0x%02x\n", x, kk);
    skip_not_equal(x, kk);
    break;
  case 0x5:
    // printf("SE_V 0x%02x 0x%02x\n", V[x], V[y]);
    skip_equal_V(x, y);
    break;
  case 0x6:
    // printf("LD 0x%02x 0x%02x\n", x, kk);
    load(x, kk);
    break;
  case 0x7:
    // printf("ADD 0x%02x 0x%02x\n", x, kk);
    add(x, kk);
    break;
  case 0x8:
    switch (n) {
    case 0x0:
      // printf("LD_V 0x%02x 0x%02x\n", x, y);
      load_V(x, y);
      break;
    case 0x1:
      // printf("OR_V 0x%02x 0x%02x\n", x, y);
      or_V(x, y);
      break;
    case 0x2:
      // printf("AND_V 0x%02x 0x%02x\n", x, y);
      and_V(x, y);
      break;
    case 0x3:
      // printf("XOR_V 0x%02x 0x%02x\n", x, y);
      xor_V(x, y);
      break;
    case 0x4:
      // printf("ADD_V 0x%02x 0x%02x\n", x, y);
      add_V(x, y);
      break;
    case 0x5:
      // printf("SUB_V 0x%02x 0x%02x\n", x, y);
      sub_V(x, y);
      break;
    case 0x6:
      // printf("SHR_V 0x%02x 0x%02x\n", x, y);
      shift_right(x);
      break;
    case 0x7:
      // printf("SUBN_V 0x%02x 0x%02x\n", x, y);
      subn_V(x, y);
      break;
    case 0xE:
      // printf("SHL_V 0x%02x 0x%02x\n", x, y);
      shift_left(x);
      break;
      // default:
      // printf("Error! 0x%04x\n", instruction);
    }
    break;
  case 0x9:
    // printf("SNE_V 0x%02x 0x%02x\n", x, y);
    skip_not_equal_V(x, y);
    break;
  case 0xA:
    // printf("LDI 0x%04x\n", nnn);
    load_I(nnn);
    break;
  case 0xB:
    // printf("JP_OFF 0x%04x\n", nnn);
    jump_with_offset(nnn);
    break;
  case 0xC:
    // printf("RND 0x%02x 0x%02x\n", x, kk);
    random_number(x, kk);
    break;
  case 0xD:
    // printf("DRW 0x%02x 0x%02x 0x%02x\n", x, y, n);
    draw(x, y, n);
    break;
  case 0xE:
    switch (kk) {
    case 0x9E:
      // printf("SP 0x%02x\n", x);
      skip_pressed(x);
      break;
    case 0xA1:
      // printf("SNP 0x%02x\n", x);
      skip_not_pressed(x);
      break;
      // default:
      // printf("Error! 0x%04x\n", instruction);
    }
    break;
  case 0xF:
    switch (kk) {
    case 0x07:
      // printf("LD V DT 0x%02x\n", x);
      load_V_DT(x);
      break;
    case 0x0A:
      // printf("WFK 0x%02x\n", x);
      wait_for_key(x);
      break;
    case 0x15:
      // printf("LD DT V 0x%02x\n", x);
      load_DT_V(x);
      break;
    case 0x18:
      // printf("LD ST V 0x%02x\n", x);
      load_ST_V(x);
      break;
    case 0x1E:
      // printf("ADD I V 0x%02x\n", x);
      add_I_V(x);
      break;
    case 0x29:
      // printf("LD CHR 0x%02x\n", x);
      load_char(x);
      break;
    case 0x33:
      // printf("LD BCD 0x%02x\n", x);
      load_bcd(x);
      break;
    case 0x55:
      // printf("STR REG 0x%02x\n", x);
      store_registers(x);
      break;
    case 0x65:
      // printf("LD REG 0x%02x\n", x);
      load_registers(x);
      break;
      // default:
      // printf("Error! 0x%04x\n", instruction);
    }
  }
}
