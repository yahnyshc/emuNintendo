#include "cpu.h"
#include "bus.h"
#include <chrono>
#include <thread>
// constructor
cpu::cpu() {}

// destructor
cpu::~cpu() {}

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

uint8_t cpu::read(uint16_t a) { return bus->cpu_read(a, false); }
void cpu::write(uint16_t a, uint8_t d) { return bus->cpu_write(a, d); }

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

bool cpu::complete() {
	return cycles == 0;
}

// This is the disassembly function. Its workings are not required for emulation.
// It is merely a convenience function to turn the binary instruction code into
// human readable form. Its included as part of the emulator because it can take
// advantage of many of the CPUs internal operations to do this.
std::map<uint16_t, std::string> cpu::disassemble(uint16_t nStart, uint16_t nStop)
{
	uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	std::map<uint16_t, std::string> mapLines;
	uint16_t line_addr = 0;

	// A convenient utility to convert variables into
	// hex strings because "modern C++"'s method with 
	// streams is atrocious
	auto hex = [](uint32_t n, uint8_t d)
		{
			std::string s(d, '0');
			for (int i = d - 1; i >= 0; i--, n >>= 4)
				s[i] = "0123456789ABCDEF"[n & 0xF];
			return s;
		};

	// Starting at the specified address we read an instruction
	// byte, which in turn yields information from the lookup table
	// as to how many additional bytes we need to read and what the
	// addressing mode is. I need this info to assemble human readable
	// syntax, which is different depending upon the addressing mode

	// As the instruction is decoded, a std::string is assembled
	// with the readable output
	while (addr <= (uint32_t)nStop)
	{
		line_addr = addr;

		// Prefix line with instruction address
		std::string sInst = "$" + hex(addr, 4) + ": ";

		// Read instruction, and get its readable name
		uint8_t opcode = bus->cpu_read(addr, true); addr++;
		sInst += instructions[opcode].name + " ";

		// Get oprands from desired locations, and form the
		// instruction based upon its addressing mode. These
		// routines mimmick the actual fetch routine of the
		// 6502 in order to get accurate data as part of the
		// instruction
		if (instructions[opcode].addrmode == &cpu::IMP)
		{
			sInst += " {IMP}";
		}
		else if (instructions[opcode].addrmode == &cpu::IMM)
		{
			value = bus->cpu_read(addr, true); addr++;
			sInst += "#$" + hex(value, 2) + " {IMM}";
		}
		else if (instructions[opcode].addrmode == &cpu::ZP0)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + " {ZP0}";
		}
		else if (instructions[opcode].addrmode == &cpu::ZPX)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", X {ZPX}";
		}
		else if (instructions[opcode].addrmode == &cpu::ZPY)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
		}
		else if (instructions[opcode].addrmode == &cpu::IZX)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + ", X) {IZX}";
		}
		else if (instructions[opcode].addrmode == &cpu::IZY)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + "), Y {IZY}";
		}
		else if (instructions[opcode].addrmode == &cpu::ABS)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = bus->cpu_read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
		}
		else if (instructions[opcode].addrmode == &cpu::ABX)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = bus->cpu_read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
		}
		else if (instructions[opcode].addrmode == &cpu::ABY)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = bus->cpu_read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
		}
		else if (instructions[opcode].addrmode == &cpu::IND)
		{
			lo = bus->cpu_read(addr, true); addr++;
			hi = bus->cpu_read(addr, true); addr++;
			sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
		}
		else if (instructions[opcode].addrmode == &cpu::REL)
		{
			value = bus->cpu_read(addr, true); addr++;
			sInst += "$" + hex(value, 2) + " [$" + hex(addr + (int8_t)value, 4) + "] {REL}";
		}

		// Add the formed string to a std::map, using the instruction's
		// address as the key. This makes it convenient to look for later
		// as the instructions are variable in length, so a straight up
		// incremental index is not sufficient.

		// std::cout << sInst << "\n";
		// std::this_thread::sleep_for(std::chrono::milliseconds(10));

		mapLines[line_addr] = sInst;
	}

	return mapLines;
}