#include "bus.h"


bus::bus() {
	// Clear RAM to avoid noise
	for (auto& i : ram) i = 0x00;

	// Connect the CPU to the bus
	olc6502.connect_bus(this);
}

bus::~bus(){}

// cpu writes data to the RAM
void bus::cpu_write(uint16_t addr, uint8_t data){
	// 8KB range
	// Originaly, nintendo RAM range is 2KB
	// So mirror every 2KB into the first one.
	if (ctrg->cpu_write(addr, data)) {
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) 
		ram[addr & 0x07FF] = data;
	else if (addr >= 0x2000 && addr <= 0x3FFF) 
		olc2C02.cpu_write(addr & 0x07FF, data); 
}

// cpu reads data from the RAM
uint8_t bus::cpu_read(uint16_t addr, bool read_only) {
	uint8_t data = 0x00;
	// 8KB range
	// Originaly, nintendo RAM range is 2KB
	// So mirror every 2KB into the first one.
	if (ctrg->cpu_read(addr, data)){ 
		return data;
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) 
		return ram[addr & 0x07FF];
	else if (addr >= 0x2000 && addr <= 0x3FFF) 
		return olc2C02.cpu_read(addr & 0x0007, read_only);

	// if reading outside the range
	return 0x00;
}

void bus::insert_cartridge(const std::shared_ptr<cartridge>& ctrg) {
	this->ctrg = ctrg;
	olc2C02.connect_cartridge(this->ctrg);
}

void bus::reset() {
	olc6502.reset();
	clock_counter = 0;
}

void bus::clock() {
	olc2C02.clock();
	// The CPU runs 3 times slower than the PPU
	if (clock_counter++ % 3 == 0)
		olc6502.clock();
}
