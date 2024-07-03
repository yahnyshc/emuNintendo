#pragma once
#include <cstdint>
#include <array>
#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"

class bus {
public:
	bus();
	~bus();

public: 
	// connected devices:
	cpu olc6502;

	ppu olc2C02;

	std::array<uint8_t, 2 * 1024> ram;

	std::shared_ptr<cartridge> ctrg;

public: // bus Read & Write
	void cpu_write(uint16_t addr, uint8_t data);
	uint8_t cpu_read(uint16_t addr, bool read_only = false);

private:
	// counts how many clock cycles have passed
	uint32_t clock_counter = 0;

public:
	// connects the cartridge class
	void insert_cartridge(const std::shared_ptr<cartridge>& ctrg);
	void reset();
	// will call to perform one system tick of the emulation
	void clock();
};