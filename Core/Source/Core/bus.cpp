#include "bus.h"

bus::bus() {
	// Clear RAM to avoid noise
	for (auto& i : ram) i = 0x00;

	// Connect the CPU to the bus
	olc6502.connect_bus(this);
}

bus::~bus(){}

// writes data to the RAM
void bus::write(uint16_t addr, uint8_t data){
	if (addr >= 0x0000 && addr <= 0xFFFF) ram[addr] = data;
}

// reads data from the RAM
uint8_t bus::read(uint16_t addr, bool read_only) {
	if (addr >= 0x0000 && addr <= 0xFFFF) return ram[addr];

	// if reading outside the range
	return 0x00;
}