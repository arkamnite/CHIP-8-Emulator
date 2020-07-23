#pragma once

#include <cstdint>

class Chip8
{
public:
	Chip8();
	uint8_t registers[16]{};	// Location on CPU for storage.
	uint8_t memory[4096]{};		// 0x000-0x1FF reserved, 0x050-0x0A0 for characters 0-F (put into memory), 0x200 onwards is for ROM memory.
	uint16_t index{};			// Stores memory addresses for operations. 16bit because max mem address is too big for 8bit.
	uint16_t pc{};				// Program counter, register that holds the address of the next instruction to execute. Opcode is 2bytes but we fetch byte from PC and PC+1.
	uint16_t stack[16]{};		// Keeps track of the execution order. Holds the PC value when CALL. HOlds 16 PCs.
	uint8_t sp{};				// Stack pointer, where did we place the most recent value? (i.e. the top).
	uint8_t delayTimer{};		// Timer = 0 ? stays zero : decrement at 60Hz.
	uint8_t soundTimer{};		// Same behaviour as DT, but tone buzzes if not zero.
	uint8_t keypad[16]{};		// Input keys 0-F. Look below.
	uint32_t video[64 * 32]{};	// Each pixel is a uint32 for SDL. :(
	uint16_t opcode;

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

private:
};