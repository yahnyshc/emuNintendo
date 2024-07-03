#include "cpu.h"
#include "bus.h"

// constructor
cpu::cpu() {}
// destructor
cpu::~cpu() {}

// reads from the bus
uint8_t cpu::read(uint16_t a) {
	return bus->cpu_read(a, false);
}

// writes to a specified address
void cpu::write(uint16_t a, uint8_t d) {
	bus->cpu_write(a, d);
}


// completely resets cpu state
void cpu::reset() {
	a = 0; x = 0; y = 0;
	stack_p = 0xFD;
	status = 0x00 | U;
	
	addr_abs = 0xFFFC;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);
	pc = (hi << 8) | lo;

	addr_rel = 0x0000; addr_abs = 0x0000;
	fetched = 0x00;

	// 8 cycles to reset
	cycles = 8;
}

void cpu::interupt(bool maskable) {
	write(0x0100 + stack_p, (pc >> 8) & 0x00FF);
	stack_p--;
	write(0x0100 + stack_p, pc & 0x00FF);
	stack_p--;

	set_flag(B, 0); set_flag(U, 1); set_flag(I, 1);
	write(0x0100 + stack_p, status);
	stack_p--;

	// read new program counter location
	addr_abs = maskable ? 0xFFFE : 0xFFFA; // fixed address
	pc = (read(addr_abs + 1) << 8) | read(addr_abs + 0);

	cycles = 7 + (maskable ? 0 : 1);
}

// maskable interupt
void cpu::irq() {
	// interrupts enabled
	if ( ! get_flag(I) ) {
		cpu::interupt(true);
	}
}

// non-maskable interrupt
void cpu::nmi() {
	cpu::interupt(false);
}

void cpu::clock() {
	if (cycles == 0) {
		opcode = read(pc);

		// set the unused status flag bit to 1
		set_flag(U, true);

		pc++;

		// Get Starting number of cycles
		cycles = instructions[opcode].cycles;

		
		uint8_t additional_cycle1 = (this->*instructions[opcode].addrmode)(); // addressing mode
		uint8_t additional_cycle2 = (this->*instructions[opcode].operate)(); // do operation

		// if both addressing mode and operation require additional cycle
		cycles += (additional_cycle1 & additional_cycle2);

		set_flag(U, true);
	}

	// increment global clock count
	clock_count++;

	// decrement the number of cycles
	cycles--;
}


uint8_t cpu::get_flag(FLAGS f) {
	return ((status & f) > 0) ? 1 : 0;
}

// Sets or clears a specific bit of the status register
void cpu::set_flag(FLAGS f, bool v) {
	v ? (status |= f) : (status &= ~f);
}

// implied addressing mode
uint8_t cpu::IMP() {
	fetched = a;
	return 0;
}


// immediate addressing mode
uint8_t cpu::IMM() {
	addr_abs = pc++;
	return 0;
}


uint8_t cpu::ZP0() {
	addr_abs = read(pc);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t cpu::ZPX() {
	addr_abs = (read(pc) + x);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t cpu::ZPY() {
	addr_abs = (read(pc) + y);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

// relative addressing mode. 
// exclusive to branch instructions.
uint8_t cpu::REL() {
	addr_rel = read(pc);
	pc++;
	if (addr_rel & 0x80) addr_rel |= 0xFF00;
	return 0;
}

// absolute addressing mode
uint8_t cpu::ABS() {
	uint16_t low = read(pc);
	pc++;
	uint16_t high = read(pc);
	pc++;

	addr_abs = (high << 8) | low;
	return 0;
}


// absolute addressing mode with X Offset
uint8_t cpu::ABX() {
	uint16_t low = read(pc);
	pc++;
	uint16_t high = read(pc);
	pc++;

	addr_abs = (high << 8) | low;
	addr_abs += x;
	return ((addr_abs & 0xFF00) != (high << 8)) ? 1 : 0;
}


// absolute addressing mode with Y Offset
uint8_t cpu::ABY() {
	uint16_t low = read(pc);
	pc++;
	uint16_t high = read(pc);
	pc++;

	addr_abs = (high << 8) | low;
	addr_abs += y;
	return ((addr_abs & 0xFF00) != (high << 8)) ? 1 : 0;
}

// indirect addressing mode
uint8_t cpu::IND() {
	uint16_t ptr_low = read(pc);
	pc++;
	uint16_t ptr_high = read(pc);
	pc++;

	uint16_t ptr = (ptr_high << 8) | ptr_low;

	if (ptr_low == 0x00FF) {
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	}
	else {
		addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
	}

	return 0;
}

// indirect X addressing mode
uint8_t cpu::IZX() {
	uint16_t t = read(pc);
	pc++;

	uint16_t low = read((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t high = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

	addr_abs = (high << 8) | low;

	return 0;
}


// indirect Y addressing mode
uint8_t cpu::IZY() {
	uint16_t t = read(pc);
	pc++;

	uint16_t low = read(t & 0x00FF);
	uint16_t high = read((t + 1) & 0x00FF);

	addr_abs = ((high << 8) | low) + y;

	return ((addr_abs & 0xFF00) != (high << 8)) ? 1 : 0;
}

uint8_t cpu::fetch() {
	if (!(instructions[opcode].addrmode == &cpu::IMP))
		fetched = read(addr_abs);
	return fetched;
}


// add with carry
uint8_t cpu::ADC() {
	fetch();
	temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)get_flag(C);

	set_flag(C, temp > 255); set_flag(Z, (temp & 0x00FF) == 0);
	set_flag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080);
	set_flag(N, temp & 0x80);
	a = temp & 0x00FF;

	// may require an additional clock cycle
	return 1;
}


// subtraction with borrow
uint8_t cpu::SBC() {
	fetch();
	uint16_t value = ((uint16_t)fetched) ^ 0x00FF;
	temp = (uint16_t)a + value + (uint16_t)get_flag(C);
	set_flag(C, temp & 0xFF00); set_flag(Z, ((temp & 0x00FF) == 0));
	set_flag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080);
	set_flag(N, temp & 0x0080);
	a = temp & 0x00FF;
	return 1;
}

// bitwise logic AND
uint8_t cpu::AND(){
	a = a & fetch();
	set_flag(Z, a == 0x00); set_flag(N, a & 0x80);
	return 1;
}

// bitwise logic XOR
uint8_t cpu::EOR() {
	a = a ^ fetch();
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 1;
}

// bitwise logic OR
uint8_t cpu::ORA() {
	a = a | fetch();
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 1;
}

// shift left
uint8_t cpu::ASL() {
	temp = (uint16_t)fetch() << 1;
	set_flag(C, (temp & 0xFF00) > 0);
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, temp & 0x80);
	if (instructions[opcode].addrmode == &cpu::IMP) {
		a = temp & 0x00FF;
	} else {
		write(addr_abs, temp & 0x00FF);
	}
	return 0;
}


// branch if carry clear
uint8_t cpu::BCC() {
	if (! get_flag(C)) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}


// branch if carry set
uint8_t cpu::BCS() {
	if (get_flag(C)) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}


// branch if equal
uint8_t cpu::BEQ() {
	if (get_flag(Z)) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}

uint8_t cpu::BIT() {
	fetch();
	temp = a & fetched;
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, fetched & (1 << 7));
	set_flag(V, fetched & (1 << 6));
	return 0;
}

// branch if negative
uint8_t cpu::BMI() {
	// not equal if result of last cpu instr was negative
	if (get_flag(N)) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}


// branch if not equal
uint8_t cpu::BNE() {
	// not equal if zero flag is set
	if ( ! get_flag(Z) ) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}

// branch if positive
uint8_t cpu::BPL() { 
	// not equal if result of last cpu instr was positive
	if ( ! get_flag(N) ) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}

// break
uint8_t cpu::BRK() {
	pc++;

	set_flag(I, 1);
	write(0x0100 + stack_p, (pc >> 8) & 0x00FF);
	stack_p--;
	write(0x0100 + stack_p, pc & 0x00FF);
	stack_p--;

	set_flag(B, 1);
	write(0x0100 + stack_p, status);
	stack_p--;
	set_flag(B, 0);

	pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);
	return 0;
}


// branch no overflow
uint8_t cpu::BVC() {
	if ( ! get_flag(V) ) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}


// branch if there was overflow
uint8_t cpu::BVS() {
	if (get_flag(V)) {
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

		pc = addr_abs;
	}
	return 0;
}

uint8_t cpu::CLC() {
	// clear carry
	set_flag(C, false);
	return 0;
}

uint8_t cpu::CLD() {
	// clear decimal
	set_flag(D, false);
	return 0;
}

uint8_t cpu::CLI() {
	// disable interupts
	set_flag(I, false);
	return 0;
}

uint8_t cpu::CLV() {
	// clear overflow bit
	set_flag(V, false);
	return 0;
}


uint8_t cpu::SEC() {
	// set carry flag
	set_flag(C, true);
	return 0;
}

uint8_t cpu::SED() {
	// set decimal flag
	set_flag(D, true);
	return 0;
}

uint8_t cpu::SEI() {
	// enable interrupts
	set_flag(I, true);
	return 0;
}

// compare accumulator
uint8_t cpu::CMP() {
	fetch();
	temp = (uint16_t)a - (uint16_t)fetched;
	set_flag(C, a >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 1;
}

uint8_t cpu::CPX() {
	fetch();
	temp = (uint16_t)x - (uint16_t)fetched;
	set_flag(C, x >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

uint8_t cpu::CPY() {
	fetch();
	temp = (uint16_t)y - (uint16_t)fetched;
	set_flag(C, y >= fetched);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

// decrement value at memory location
uint8_t cpu::DEC() {
	temp = fetch() - 1;
	write(addr_abs, temp & 0x00FF);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

// increment value at memory location
uint8_t cpu::INC() {
	temp = fetch() + 1;
	write(addr_abs, temp & 0x00FF);
	set_flag(Z, (temp & 0x00FF) == 0x0000);
	set_flag(N, temp & 0x0080);
	return 0;
}

// decrement X register
uint8_t cpu::DEX() {
	x--;
	set_flag(Z, x == 0x00); set_flag(N, x & 0x80);
	return 0;
}

// decrement Y register
uint8_t cpu::DEY() {
	y--;
	set_flag(Z, y == 0x00); set_flag(N, y & 0x80);
	return 0;
}

// increment X register
uint8_t cpu::INX() {
	x++;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// increment Y register
uint8_t cpu::INY() {
	y++;
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}

// jump to location
uint8_t cpu::JMP() {
	pc = addr_abs;
	return 0;
}

// jump To sub-routine
uint8_t cpu::JSR() {
	pc--;

	write(0x0100 + stack_p, (pc >> 8) & 0x00FF);
	stack_p--;
	write(0x0100 + stack_p, pc & 0x00FF);
	stack_p--;

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

// load the Y register
uint8_t cpu::LDY() {
	y = fetch();
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 1;
}

uint8_t cpu::LSR() {
	fetch();
	set_flag(C, fetched & 0x0001);
	temp = fetched >> 1;
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

// push accumulator to stack
uint8_t cpu::PHA() {
	write(0x0100 + stack_p, a);
	stack_p--;
	return 0;
}

// push status register to stack
uint8_t cpu::PHP() {
	write(0x0100 + stack_p, status | B | U);
	set_flag(B, 0); set_flag(U, 0);
	stack_p--;
	return 0;
}

// pop accumulator from stack
uint8_t cpu::PLA() {
	stack_p++;
	a = read(0x0100 + stack_p);
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 0;
}

// pop status register from stack
uint8_t cpu::PLP() {
	stack_p++;
	status = read(0x0100 + stack_p);
	set_flag(U, 1);
	return 0;
}

uint8_t cpu::ROL() {
	fetch();
	temp = (uint16_t)(fetched << 1) | get_flag(C);
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
	fetch();
	temp = (uint16_t)(get_flag(C) << 7) | (fetched >> 1);
	set_flag(C, fetched & 0x01);
	set_flag(Z, (temp & 0x00FF) == 0x00);
	set_flag(N, temp & 0x0080);
	if (instructions[opcode].addrmode == &cpu::IMP)
		a = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t cpu::RTI() {
	stack_p++;
	status = read(0x0100 + stack_p);
	status &= ~B;
	status &= ~U;

	stack_p++;
	pc = (uint16_t)read(0x0100 + stack_p);
	stack_p++;
	pc |= (uint16_t)read(0x0100 + stack_p) << 8;
	return 0;
}

uint8_t cpu::RTS() {
	stack_p++;
	pc = (uint16_t)read(0x0100 + stack_p);
	stack_p++;
	pc |= (uint16_t)read(0x0100 + stack_p) << 8;

	pc++;
	return 0;
}

// store accumulator at address
uint8_t cpu::STA() {
	write(addr_abs, a);
	return 0;
}

// store X register at address
uint8_t cpu::STX() {
	write(addr_abs, x);
	return 0;
}

// store Y register at address
uint8_t cpu::STY() {
	write(addr_abs, y);
	return 0;
}

// move accumulator to X register
uint8_t cpu::TAX() {
	x = a;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// move accumulator to Y register
uint8_t cpu::TAY() {
	y = a;
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}

// move stack pointer to X register
uint8_t cpu::TSX() {
	x = stack_p;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}

// move X register to accumulator
uint8_t cpu::TXA() {
	a = x;
	set_flag(Z, a == 0x00);
	set_flag(N, a & 0x80);
	return 0;
}

// move X register to stack pointer
uint8_t cpu::TXS() {
	stack_p = x;
	return 0;
}

// move Y register to accumulator
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

bool cpu::complete() { return cycles == 0; }

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
	// byte, which in turn yields information from the instructions table
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
		mapLines[line_addr] = sInst;
	}

	return mapLines;
}