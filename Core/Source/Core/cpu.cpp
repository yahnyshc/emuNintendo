#include "cpu.h"
#include "bus.h"

// constructor
cpu::cpu() {
	instructions = {
		// {Mnemonic, operation, addressmode, number of clock cycles}
		{ "BRK", &cpu::BRK, &cpu::IMM, 7 },{ "ORA", &cpu::ORA, &cpu::IZX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 3 },{ "ORA", &cpu::ORA, &cpu::ZP0, 3 },{ "ASL", &cpu::ASL, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "PHP", &cpu::PHP, &cpu::IMP, 3 },{ "ORA", &cpu::ORA, &cpu::IMM, 2 },{ "ASL", &cpu::ASL, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ABS, 4 },{ "ASL", &cpu::ASL, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BPL", &cpu::BPL, &cpu::REL, 2 },{ "ORA", &cpu::ORA, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ZPX, 4 },{ "ASL", &cpu::ASL, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "CLC", &cpu::CLC, &cpu::IMP, 2 },{ "ORA", &cpu::ORA, &cpu::ABY, 4 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ABX, 4 },{ "ASL", &cpu::ASL, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "JSR", &cpu::JSR, &cpu::ABS, 6 },{ "AND", &cpu::AND, &cpu::IZX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "BIT", &cpu::BIT, &cpu::ZP0, 3 },{ "AND", &cpu::AND, &cpu::ZP0, 3 },{ "ROL", &cpu::ROL, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "PLP", &cpu::PLP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::IMM, 2 },{ "ROL", &cpu::ROL, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "BIT", &cpu::BIT, &cpu::ABS, 4 },{ "AND", &cpu::AND, &cpu::ABS, 4 },{ "ROL", &cpu::ROL, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BMI", &cpu::BMI, &cpu::REL, 2 },{ "AND", &cpu::AND, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::ZPX, 4 },{ "ROL", &cpu::ROL, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "SEC", &cpu::SEC, &cpu::IMP, 2 },{ "AND", &cpu::AND, &cpu::ABY, 4 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::ABX, 4 },{ "ROL", &cpu::ROL, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "RTI", &cpu::RTI, &cpu::IMP, 6 },{ "EOR", &cpu::EOR, &cpu::IZX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 3 },{ "EOR", &cpu::EOR, &cpu::ZP0, 3 },{ "LSR", &cpu::LSR, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "PHA", &cpu::PHA, &cpu::IMP, 3 },{ "EOR", &cpu::EOR, &cpu::IMM, 2 },{ "LSR", &cpu::LSR, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "JMP", &cpu::JMP, &cpu::ABS, 3 },{ "EOR", &cpu::EOR, &cpu::ABS, 4 },{ "LSR", &cpu::LSR, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BVC", &cpu::BVC, &cpu::REL, 2 },{ "EOR", &cpu::EOR, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "EOR", &cpu::EOR, &cpu::ZPX, 4 },{ "LSR", &cpu::LSR, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "CLI", &cpu::CLI, &cpu::IMP, 2 },{ "EOR", &cpu::EOR, &cpu::ABY, 4 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "EOR", &cpu::EOR, &cpu::ABX, 4 },{ "LSR", &cpu::LSR, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "RTS", &cpu::RTS, &cpu::IMP, 6 },{ "ADC", &cpu::ADC, &cpu::IZX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 3 },{ "ADC", &cpu::ADC, &cpu::ZP0, 3 },{ "ROR", &cpu::ROR, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "PLA", &cpu::PLA, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::IMM, 2 },{ "ROR", &cpu::ROR, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "JMP", &cpu::JMP, &cpu::IND, 5 },{ "ADC", &cpu::ADC, &cpu::ABS, 4 },{ "ROR", &cpu::ROR, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BVS", &cpu::BVS, &cpu::REL, 2 },{ "ADC", &cpu::ADC, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::ZPX, 4 },{ "ROR", &cpu::ROR, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "SEI", &cpu::SEI, &cpu::IMP, 2 },{ "ADC", &cpu::ADC, &cpu::ABY, 4 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::ABX, 4 },{ "ROR", &cpu::ROR, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "STA", &cpu::STA, &cpu::IZX, 6 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "STY", &cpu::STY, &cpu::ZP0, 3 },{ "STA", &cpu::STA, &cpu::ZP0, 3 },{ "STX", &cpu::STX, &cpu::ZP0, 3 },{ "???", &cpu::XXX, &cpu::IMP, 3 },
		{ "DEY", &cpu::DEY, &cpu::IMP, 2 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "TXA", &cpu::TXA, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "STY", &cpu::STY, &cpu::ABS, 4 },{ "STA", &cpu::STA, &cpu::ABS, 4 },{ "STX", &cpu::STX, &cpu::ABS, 4 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "BCC", &cpu::BCC, &cpu::REL, 2 },{ "STA", &cpu::STA, &cpu::IZY, 6 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "STY", &cpu::STY, &cpu::ZPX, 4 },{ "STA", &cpu::STA, &cpu::ZPX, 4 },{ "STX", &cpu::STX, &cpu::ZPY, 4 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "TYA", &cpu::TYA, &cpu::IMP, 2 },{ "STA", &cpu::STA, &cpu::ABY, 5 },{ "TXS", &cpu::TXS, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "???", &cpu::NOP, &cpu::IMP, 5 },{ "STA", &cpu::STA, &cpu::ABX, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "LDY", &cpu::LDY, &cpu::IMM, 2 },{ "LDA", &cpu::LDA, &cpu::IZX, 6 },{ "LDX", &cpu::LDX, &cpu::IMM, 2 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "LDY", &cpu::LDY, &cpu::ZP0, 3 },{ "LDA", &cpu::LDA, &cpu::ZP0, 3 },{ "LDX", &cpu::LDX, &cpu::ZP0, 3 },{ "???", &cpu::XXX, &cpu::IMP, 3 },
		{ "TAY", &cpu::TAY, &cpu::IMP, 2 },{ "LDA", &cpu::LDA, &cpu::IMM, 2 },{ "TAX", &cpu::TAX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "LDY", &cpu::LDY, &cpu::ABS, 4 },{ "LDA", &cpu::LDA, &cpu::ABS, 4 },{ "LDX", &cpu::LDX, &cpu::ABS, 4 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "BCS", &cpu::BCS, &cpu::REL, 2 },{ "LDA", &cpu::LDA, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "LDY", &cpu::LDY, &cpu::ZPX, 4 },{ "LDA", &cpu::LDA, &cpu::ZPX, 4 },{ "LDX", &cpu::LDX, &cpu::ZPY, 4 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "CLV", &cpu::CLV, &cpu::IMP, 2 },{ "LDA", &cpu::LDA, &cpu::ABY, 4 },{ "TSX", &cpu::TSX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "LDY", &cpu::LDY, &cpu::ABX, 4 },{ "LDA", &cpu::LDA, &cpu::ABX, 4 },{ "LDX", &cpu::LDX, &cpu::ABY, 4 },{ "???", &cpu::XXX, &cpu::IMP, 4 },
		{ "CPY", &cpu::CPY, &cpu::IMM, 2 },{ "CMP", &cpu::CMP, &cpu::IZX, 6 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "CPY", &cpu::CPY, &cpu::ZP0, 3 },{ "CMP", &cpu::CMP, &cpu::ZP0, 3 },{ "DEC", &cpu::DEC, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "INY", &cpu::INY, &cpu::IMP, 2 },{ "CMP", &cpu::CMP, &cpu::IMM, 2 },{ "DEX", &cpu::DEX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 2 },
		{ "CPY", &cpu::CPY, &cpu::ABS, 4 },{ "CMP", &cpu::CMP, &cpu::ABS, 4 },{ "DEC", &cpu::DEC, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BNE", &cpu::BNE, &cpu::REL, 2 },{ "CMP", &cpu::CMP, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "CMP", &cpu::CMP, &cpu::ZPX, 4 },{ "DEC", &cpu::DEC, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "CLD", &cpu::CLD, &cpu::IMP, 2 },{ "CMP", &cpu::CMP, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "CMP", &cpu::CMP, &cpu::ABX, 4 },{ "DEC", &cpu::DEC, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "CPX", &cpu::CPX, &cpu::IMM, 2 },{ "SBC", &cpu::SBC, &cpu::IZX, 6 },{ "???", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "CPX", &cpu::CPX, &cpu::ZP0, 3 },{ "SBC", &cpu::SBC, &cpu::ZP0, 3 },{ "INC", &cpu::INC, &cpu::ZP0, 5 },{ "???", &cpu::XXX, &cpu::IMP, 5 },
		{ "INX", &cpu::INX, &cpu::IMP, 2 },{ "SBC", &cpu::SBC, &cpu::IMM, 2 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::SBC, &cpu::IMP, 2 },
		{ "CPX", &cpu::CPX, &cpu::ABS, 4 },{ "SBC", &cpu::SBC, &cpu::ABS, 4 },{ "INC", &cpu::INC, &cpu::ABS, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "BEQ", &cpu::BEQ, &cpu::REL, 2 },{ "SBC", &cpu::SBC, &cpu::IZY, 5 },{ "???", &cpu::XXX, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 8 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "SBC", &cpu::SBC, &cpu::ZPX, 4 },{ "INC", &cpu::INC, &cpu::ZPX, 6 },{ "???", &cpu::XXX, &cpu::IMP, 6 },
		{ "SED", &cpu::SED, &cpu::IMP, 2 },{ "SBC", &cpu::SBC, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
		{ "???", &cpu::NOP, &cpu::IMP, 4 },{ "SBC", &cpu::SBC, &cpu::ABX, 4 },{ "INC", &cpu::INC, &cpu::ABX, 7 },{ "???", &cpu::XXX, &cpu::IMP, 7 },
	};
}

// destructor
cpu::~cpu() {}

uint8_t cpu::read(uint16_t a) { return bus->read(a, false); }
void cpu::write(uint16_t a, uint8_t d) { return bus->write(a, d); }

// this is implemented in a behavioural fashion because the real hardware would execute this
// in multiple clock cycle, however I am doing everying at one:  when the cycles == 0
void cpu::clock() {
	// this will be executed one the previous clock cycle has ended
	if (cycles == 0) {
		opcode = read(pc++); // reads one byte which will be used to index into instructions

		// Get starting number of cycles
		cycles = instructions[opcode].cycles;

		uint8_t addr_mode_cycle = (this->*instructions[opcode].addrmode)();

		uint8_t operation_cycle = (this->*instructions[opcode].operate)();

		// if both address mode and operation require additional clock cycle
		cycles += (addr_mode_cycle && operation_cycle);
	}

	cycles--;
}

// returns the value of a specific bit of the status register
uint8_t cpu::get_flag(FLAGS f) {
	return ((status & f) > 0) ? 1 : 0;
}

// sets or clears a specific bit of the status register
void cpu::set_flag(FLAGS f, bool v) {
	v ? status |= f : status &= ~f;
}

// implied addressing mode
uint8_t cpu::IMP() {
	fetched = a;
	return 0;
}

// immdiate, data is suplied as part of the instruction
uint8_t cpu::IMM() {
	addr_abs = pc++;
	return 0;
}

// zero page addressing
uint8_t cpu::ZP0() {
	addr_abs = read(pc++);
	addr_abs &= 0x00FF;
	return 0;
}

// x offsetting
uint8_t cpu::ZPX() {
	addr_abs = (read(pc++) + x);
	addr_abs &= 0x00FF;
	return 0;
}

// y offsetting
uint8_t cpu::ZPY() {
	addr_abs = (read(pc++) + y);
	addr_abs &= 0x00FF;
	return 0;
}

// use absolute address: lo + hi bytes
uint8_t cpu::ABS() {
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);

	addr_abs = (hi << 8) | lo;
	return 0;
}

// use absolute address: lo + hi bytes + X offset
uint8_t cpu::ABX() {
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);

	addr_abs = ((hi << 8) | lo) + x;

	// additional clock cycle might be needed
	// if the page number has changed
	return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// use absolute address: lo + hi bytes + Y offset
uint8_t cpu::ABY() {
	uint16_t lo = read(pc++);
	uint16_t hi = read(pc++);

	addr_abs = ((hi << 8) | lo) + y;

	// additional clock cycle might be needed
	// if the page number has changed
	return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// indirect addressing
uint8_t cpu::IND() {
	uint16_t ptr_low = read(pc++);
	uint16_t ptr_high = read(pc++);

	uint16_t ptr = (ptr_high << 8) | ptr_low;

	if (ptr_low == 0x00FF)
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	else
		addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);

	return 0;
}

uint8_t cpu::IZX(){
	// address from the program counter is somewhere in the zero page
	uint16_t t = read(pc++);

	uint16_t low = read((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t high = read((uint16_t)(t + (uint16_t)x+1) & 0x00FF);

	addr_abs = (high << 8) | low;

	return 0;
}

uint8_t cpu::IZY() {
	// address from the program counter is somewhere in the zero page
	uint16_t t = read(pc++);

	uint16_t low = read(t & 0x00FF);
	uint16_t high = read((t + 1) & 0x00FF);

	addr_abs = ((high << 8) | low) + y;

	// additional clock cycle might be needed
	// if the page number has changed
	return ((addr_abs & 0xFF00) != (high << 8)) ? 1 : 0;
}

uint8_t cpu::REL() {
	addr_rel = read(pc++);
	// if relative address is negative
	if (addr_rel & 0x80)
		// make all of the 1st byte bits set to 1 so that it is 
		// guarteed to be negative in the arithmetic
		addr_rel |= 0xFF00;
	return 0;
}

// Instructions

uint8_t cpu::fetch() {
	// if implied mode: there is no data to fetch 
	if (instructions[opcode].addrmode != &cpu::IMP)
		fetched = read(addr_abs);
	return fetched;
}

uint8_t cpu::AND() {
	// & between acumulator and fetched data
	a = a & fetch();
	// set the flag whether accumulator is zero
	set_flag(Z, a == 0x00);
	// check if it is negative
	set_flag(N, a & 0x80);
	// might require additional clock cycle if addressing mode also needs it
	return 1;
}

// branch if carry set
uint8_t cpu::BCS() {
	// if carry bit is set
	if (get_flag(C) == 1) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if carry clear
uint8_t cpu::BCC() {
	// if carry bit is set
	if (get_flag(C) == 0) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if equal
uint8_t cpu::BEQ() {
	// if zero bit is 0
	if (get_flag(Z) == 0) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if negative
uint8_t cpu::BMI() {
	// if negative bit is one
	if (get_flag(N) == 1) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if not equal
uint8_t cpu::BNE() {
	// if zero bit is zero
	if (get_flag(Z) == 0) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if positive
uint8_t cpu::BPL() {
	// if negative bit is zero
	if (get_flag(N) == 0) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if overflow
uint8_t cpu::BVC() {
	// if overflow is set to 0
	if (get_flag(V) == 0) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if not overflow
uint8_t cpu::BVS() {
	// if not overflow is set to 1
	if (get_flag(V) == 1) {
		// cycles penalty
		cycles++;
		addr_abs = pc + addr_rel;

		// if crossing page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// clear the carry bit
uint8_t cpu::CLC() {
	set_flag(C, false);
	return 0;
}

uint8_t cpu::CLD() {
	set_flag(D, false);
	return 0;
}

// disable interupts
uint8_t cpu::CLI(){
	set_flag(I, false);
	return 0;
}

// clear overflow flag
uint8_t cpu::CLV(){
	set_flag(V, false);
	return 0;
}

uint8_t cpu::ADC() {
	uint16_t temp = (uint16_t)a + (uint16_t)fetch() + (uint16_t)get_flag(C);
	set_flag(C, temp > 255);
	set_flag(Z, (temp & 0x00FF) == 0);
	set_flag(N, temp & 0x80);
	// setting the overflow flag using formula derived from the truth table.
	set_flag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080);
	a = temp & 0x00FF;
	// might require additional clock cycle.
	return 1;
}

uint8_t cpu::SBC() {
	uint16_t inverted_value = ((uint16_t)fetch()) ^ 0x00FF;

	// Notice this is exactly the same as addition from here!
	uint16_t temp = (uint16_t)a + inverted_value + (uint16_t)get_flag(C);
	set_flag(C, temp & 0xFF00);
	set_flag(Z, ((temp & 0x00FF) == 0));
	set_flag(N, temp & 0x0080);
	// setting the overflow flag using formula derived from the truth table.
	set_flag(V, (temp ^ (uint16_t)a) & (temp ^ inverted_value) & 0x0080);
	// might require additional clock cycle.
	a = temp & 0x00FF;
	return 1;
}

// stack instructions

uint8_t cpu::PHA() {
	write(0x0100 + stack_p, a);
	// move stack pointer down
	stack_p--;
	return 0;
}

uint8_t cpu::PLA() {
	a = read(0x0100 + ++stack_p);
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 0;
}

// interupts

void cpu::reset() {
	a = 0; x = 0; y = 0;
	stack_p = 0xFD; status = 0x00 | U;

	// address to look for pc address
	addr_abs = 0xFFFC;
	uint16_t low = read(addr_abs + 0);
	uint16_t high = read(addr_abs + 1);

	pc = (high << 8) | low;

	addr_rel = 0x0000; addr_abs = 0x0000; 
	fetched = 0x00;

	cycles = 8;
}

void cpu::irq() {
	// if interupts not disabpled
	if (get_flag(I) == 0) {
		// Push the pc to the stack.
		write(0x0100 + stack_p--, (pc >> 8) & 0x00FF);
		write(0x0100 + stack_p--, pc & 0x00FF);

		set_flag(B, 0); set_flag(U, 1); set_flag(I, 1);
		write(0x0100 + stack_p--, status);

		// interupt handler location
		addr_abs = 0xFFFE;
		uint16_t low = read(addr_abs + 0);
		uint16_t high = read(addr_abs + 1);
		pc = (high << 8) | low;

		cycles = 7;
	}
}

void cpu::nmi() {
	// Push the pc to the stack.
	write(0x0100 + stack_p--, (pc >> 8) & 0x00FF);
	write(0x0100 + stack_p--, pc & 0x00FF);

	set_flag(B, 0); set_flag(U, 1); set_flag(I, 1);
	write(0x0100 + stack_p--, status);

	// interupt handler location
	addr_abs = 0xFFFE;
	uint16_t low = read(addr_abs + 0);
	uint16_t high = read(addr_abs + 1);
	pc = (high << 8) | low;

	// one more cycle compared to mascable interupt
	cycles = 8;
}

// restore state before interupt
uint8_t cpu::RTI() {
	status = read(0x0100 + ++stack_p);
	status &= ~B;
	status &= ~U;

	pc = (uint16_t)read(0x0100 + ++stack_p);
	pc |= (uint16_t)read(0x0100 + ++stack_p) << 8;
	return 0;
}

// arithmetic shift left
uint8_t cpu::ASL() {
	uint16_t temp = (uint16_t)fetch() << 1;
	set_flag(C, (temp & 0xFF00) > 0);
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, temp & 0x80);
	if (instructions[opcode].addrmode == &cpu::IMP)
		a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t cpu::BIT() {
	uint16_t temp = a & fetch();
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, fetched & (1 << 7));
	set_flag(V, fetched & (1 << 6));
	return 0;
}

// break
uint8_t cpu::BRK(){
	pc++;

	set_flag(I, 1);
	write(0x0100 + stack_p--, (pc >> 8) & 0x00FF);
	write(0x0100 + stack_p--, pc & 0x00FF);

	set_flag(B, 1);
	write(0x0100 + stack_p--, status);
	set_flag(B, 0);

	pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);
	return 0;
}

// compare accumulator
uint8_t cpu::CMP() {
	uint16_t temp = (uint16_t)a - (uint16_t)fetch();
	set_flag(C, a >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 1;
}

// compare x register
uint8_t cpu::CPX() {
	uint16_t temp = (uint16_t)x - (uint16_t)fetch();
	set_flag(C, x >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

// compare x register
uint8_t cpu::CPY() {
	fetch();
	uint16_t temp = (uint16_t)y - (uint16_t)fetched;
	set_flag(C, y >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

// decrement value at given location
uint8_t cpu::DEC() {
	uint16_t temp = fetch() - 1;
	write(addr_abs, temp & 0x00FF);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}


// decrement x register
uint8_t cpu::DEX() {
	set_flag(Z, --x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}


// decrement y register
uint8_t cpu::DEY() {
	set_flag(Z, --y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}


// bitwise logic XOR
uint8_t cpu::EOR() {
	a = a ^ fetch();
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 1;
}

// increment value at memory location
uint8_t cpu::INC() {
	uint16_t temp = fetch() + 1;
	write(addr_abs, temp & 0x00FF);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}


// increment x register
uint8_t cpu::INX(){
	set_flag(Z, ++x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// increment y register
uint8_t cpu::INY() {
	set_flag(Z, ++y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}

// jump to location
uint8_t cpu::JMP() {
	pc = addr_abs;
	return 0;
}

// jump to sub-routine
uint8_t cpu::JSR() {
	write(0x0100 + stack_p--, (--pc >> 8) & 0x00FF);
	write(0x0100 + stack_p--, pc & 0x00FF);
	pc = addr_abs;
	return 0;
}

// load the accumulator
uint8_t cpu::LDA() {
	a = fetch();
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 1;
}

// load the x register
uint8_t cpu::LDX() {
	x = fetch();
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 1;
}

// load the y register
uint8_t cpu::LDY() {
	y = fetch();
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 1;
}

uint8_t cpu::LSR() {
	set_flag(C, fetch() & 0x0001);
	uint16_t temp = fetched >> 1;
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	if (instructions[opcode].addrmode == &cpu::IMP)
		a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t cpu::NOP() {
	switch (opcode) {
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}
	return 0;
}

// bitwise logic OR
uint8_t cpu::ORA() {
	a = a | fetch();
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 1;
}

// push status register to stack
uint8_t cpu::PHP() {
	write(0x0100 + stack_p--, status | B | U);
	set_flag(B, 0);
	set_flag(U, 0);
	return 0;
}

// pop status register off stack
uint8_t cpu::PLP() {
	status = read(0x0100 + ++stack_p);
	set_flag(U, 1);
	return 0;
}

uint8_t cpu::ROL() {
	uint16_t temp = (uint16_t)(fetch() << 1) | get_flag(C);
	set_flag(C, temp & 0xFF00);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	if (instructions[opcode].addrmode == &cpu::IMP)
		a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t cpu::ROR() {
	uint16_t temp = (uint16_t)(get_flag(C) << 7) | (fetch() >> 1);
	set_flag(C, fetched & 0x01);
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, temp & 0x0080);
	if (instructions[opcode].addrmode == &cpu::IMP)
		a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t cpu::RTS() {
	pc = (uint16_t)read(0x0100 + ++stack_p);
	pc |= (uint16_t)read(0x0100 + ++stack_p) << 8;
	pc++;
	return 0;
}

// set carry flag
uint8_t cpu::SEC() {
	set_flag(C, true);
	return 0;
}

// set decimal flag
uint8_t cpu::SED() {
	set_flag(D, true);
	return 0;
}

// set interrupt flag / enable interrupts
uint8_t cpu::SEI() {
	set_flag(I, true);
	return 0;
}

// store accumulator at address
uint8_t cpu::STA() {
	write(addr_abs, a);
	return 0;
}

// store x register at address
uint8_t cpu::STX() {
	write(addr_abs, x);
	return 0;
}

// store y register at address
uint8_t cpu::STY() {
	write(addr_abs, y);
	return 0;
}

// transfer accumulator to x register
uint8_t cpu::TAX() {
	x = a;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// transfer accumulator to y register
uint8_t cpu::TAY() {
	y = a;
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}

// transfer stack pointer to x register
uint8_t cpu::TSX() {
	x = stack_p;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// transfer x register to accumulator
uint8_t cpu::TXA() {
	a = x;
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 0;
}

// transfer x register to stack pointer
uint8_t cpu::TXS(){
	stack_p = x;
	return 0;
}


// transfer y register to accumulator
uint8_t cpu::TYA() {
	a = y;
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 0;
}

// for illegal opcodes
uint8_t cpu::XXX() {
	return 0;
}