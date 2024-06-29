#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

#include "Core/cartridgeMapper.h"

#define KB 1024

class cartridge {
public:
	cartridge(const std::string& filename);
	~cartridge();

public:
	bool ImageValid();

	enum MIRROR
	{
		HORIZONTAL,
		VERTICAL,
		ONESCREEN_LO,
		ONESCREEN_HI,
	} mirror = HORIZONTAL;
public:
	bool cpu_read(uint16_t addr, uint8_t& data);
	bool cpu_write(uint16_t addr, uint8_t data);

	bool ppu_read(uint16_t addr, uint8_t& data);
	bool ppu_write(uint16_t addr, uint8_t data);

	uint8_t program_banks_n = 0;

private:
	bool image_valid = false;
	std::vector<uint8_t> program_memory;
	std::vector<uint8_t> pattern_memory;

	// which cartridge mapper is used
	uint8_t mapper_id = 0;
	// number of program banks sections
	
	// number of program characters banks sections
	uint8_t pattern_banks_n = 0;

	cartridgeMapper pMapper;
};

