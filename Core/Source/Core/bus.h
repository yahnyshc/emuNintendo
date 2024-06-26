#pragma once
#include <cstdint>
#include "cpu.h"
#include <array>

class bus {
public:
	bus();
	~bus();

public: 
	// connected devices:
	cpu olc6502;
	std::array<uint8_t, 64 * 1024> ram;

public: // bus Read & Write
	void write(uint16_t addr, uint8_t data);
	uint8_t read(uint16_t addr, bool read_only = false);
};



