#pragma once
#include <cstdint>

class cartridgeMapper {
public:
	cartridgeMapper() = default;  // Default constructor
	cartridgeMapper(uint8_t program_banks, uint8_t pattern_banks);
	~cartridgeMapper();

public:
	// transform CPU bus address into PRG ROM offset
	bool cpu_map_read(uint16_t addr, uint32_t& mapped_addr);
	bool cpu_map_write(uint16_t addr, uint32_t& mapped_addr);
	// yransform PPU bus address into CHR ROM offset
	bool ppu_map_read(uint16_t addr, uint32_t& mapped_addr);
	bool ppu_map_write(uint16_t addr, uint32_t& mapped_addr);


private:
	uint8_t program_banks_n = 0;
	uint8_t pattern_banks_n = 0;
};

