#include "cartridgeMapper.h"
#include <iostream>

cartridgeMapper::cartridgeMapper(uint8_t program_banks, uint8_t pattern_banks){
	program_banks_n = program_banks;
	pattern_banks_n = pattern_banks;
}

cartridgeMapper::~cartridgeMapper(){}

bool cartridgeMapper::cpu_map_read(uint16_t addr, uint32_t& mapped_addr){
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (program_banks_n > 1)
			mapped_addr = addr & 0x7FFF;
		else
			mapped_addr = addr & 0x3FFF; // mirroring because 16K
		return true;
	}
	
	return false;
}

bool cartridgeMapper::cpu_map_write(uint16_t addr, uint32_t& mapped_addr){
	if (addr >= 0x8000 && addr <= 0xFFFF) {
		if (program_banks_n > 1)
			mapped_addr = addr & 0x7FFF;
		else
			mapped_addr = addr & 0x3FFF; // mirroring because 16K
		return true;
	}
	
	return false;
}

bool cartridgeMapper::ppu_map_read(uint16_t addr, uint32_t& mapped_addr){
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		mapped_addr = addr;
		return true;
	}
	
	return false;
}

bool cartridgeMapper::ppu_map_write(uint16_t addr, uint32_t& mapped_addr){
	return false;
}
