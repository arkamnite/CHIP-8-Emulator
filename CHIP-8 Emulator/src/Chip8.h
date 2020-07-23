#pragma once

#include <cstdint>
#include <chrono>
#include <random>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
	Chip8();
	uint8_t keypad[KEY_COUNT]{};		// Input keys 0-F. Look below.
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};	// Each pixel is a uint32 for SDL. :(

/* Keypad Binding
	Keypad       Keyboard
	+-+-+-+-+    +-+-+-+-+
	|1|2|3|C|    |1|2|3|4|
	+-+-+-+-+    +-+-+-+-+
	|4|5|6|D|    |Q|W|E|R|
	+-+-+-+-+ => +-+-+-+-+
	|7|8|9|E|    |A|S|D|F|
	+-+-+-+-+    +-+-+-+-+
	|A|0|B|F|    |Z|X|C|V|
	+-+-+-+-+    +-+-+-+-+
*/

	void LoadROM(char const* filename);
	void Cycle();

private:
	void Table0();
	void Table8();
	void TableE();
	void TableF();

	// Do nothing
	void OP_NULL();

	// CLS: Clear the display. Entire video buffer set to 0s.
	void OP_00E0();

	// RET: Return from a subroutine.
	void OP_00EE();

	// JP addr: Jump to location nnn. Interpreter sets the pc to nnn.
	void OP_1nnn();

	// CALL addr: Call subroutine at nnn.
	void OP_2nnn();

	// SE Vx, byte: Skip next instruction if Vx = kk.
	void OP_3xkk();

	// SNE Vx, byte: Skip next instruction if Vx != kk.
	void OP_4xkk();

	// SE Vx, Vy: Skip next instruction if Vx = Vy.
	void OP_5xy0();

	// LD Vx, byte: Set Vx = kk.
	void OP_6xkk();

	// ADD Vx, byte: Set Vx = Vx + kk.
	void OP_7xkk();

	// LD Vx, Vy: Set Vx = Vy.
	void OP_8xy0();

	// OR Vx, Vy: Set Vx = Vx OR Vy.
	void OP_8xy1();

	// AND Vx, Vy: Set Vx = Vx AND Vy.
	void OP_8xy2();

	// XOR Vx, Vy: Set Vx = Vx XOR Vy.
	void OP_8xy3();

	// ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry. Has an overflow tag; ANS > 255 ? VF 1 : 0. Lowest 8 bits are 
	// kept and stored in Vx.
	void OP_8xy4();

	// SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
	void OP_8xy5();

	// SHR Vx: Set Vx = Vx SHR 1. Right shift (/2), and save least significant bit in register VF.
	void OP_8xy6();

	// SUBN Vx, Vy: Vx = Vy - Vx.
	void OP_8xy7();

	// SHL Vx {, Vy}: Set Vx = Vx SHL 1. Saves most significant bit after a shift left (*2)
	void OP_8xyE();

	// SNE Vx, Vy: Skip next instruction if Vx != Vy.
	void OP_9xy0();

	// LD I, addr: Set I = nnn.
	void OP_Annn();

	// JP V0, addr: Jump to location nnn + V0.
	void OP_Bnnn();

	// RND Vx, byte: Set Vx = random byte AND kk.
	void OP_Cxkk();

	// DRW Vx, Vy, nibble: Display n-byte sprite starting at memloc I at (Vx, Vy), set VF = collision.
	void OP_Dxyn();

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT: Set Vx = Delay timer value.
	void OP_Fx07();

	// LD Vx, K: Wait for keypress, store value of key in Vx.
	void OP_Fx0A();

	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx: Set I = I + Vx.
	void OP_Fx1E();

	// LD F, Vx: Set I = Location of sprite for digit Vx.
	void OP_Fx29();

	// LD B, Vx: Store BCD rep of Vx in memlocs I, I+1, I+2. 
	void OP_Fx33();

	// LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
	void OP_Fx55();

	// LD Vx, [I]: Read registers V0 through Vx from memory starting at loction I.
	void OP_Fx65();

	uint8_t registers[REGISTER_COUNT]{};	// Location on CPU for storage.
	uint8_t memory[MEMORY_SIZE]{};		// 0x000-0x1FF reserved, 0x050-0x0A0 for characters 0-F (put into memory), 0x200 onwards is for ROM memory.
	uint16_t index{};			// Stores memory addresses for operations. 16bit because max mem address is too big for 8bit.
	uint16_t pc{};				// Program counter, register that holds the address of the next instruction to execute. Opcode is 2bytes but we fetch byte from PC and PC+1.
	uint16_t stack[STACK_LEVELS]{};		// Keeps track of the execution order. Holds the PC value when CALL. HOlds 16 PCs.
	uint8_t sp{};				// Stack pointer, where did we place the most recent value? (i.e. the top).
	uint8_t delayTimer{};		// Timer = 0 ? stays zero : decrement at 60Hz.
	uint8_t soundTimer{};		// Same behaviour as DT, but tone buzzes if not zero.
	uint16_t opcode;

	std::default_random_engine randGen;
	std::uniform_int_distribution<unsigned int> randByte;

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1]{ &Chip8::OP_NULL };
	Chip8Func table0[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func table8[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableE[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableF[0x65 + 1]{ &Chip8::OP_NULL };
};