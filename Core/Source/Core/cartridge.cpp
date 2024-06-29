#include "cartridge.h"
#include "cartridgeMapper.h"
#include <iostream>

cartridge::cartridge(const std::string& filename) {
	// iNES Format Header
	struct HEADER {
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header;

	image_valid = false;

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::binary);
	if (ifs.is_open()) {
		// Read file header
		ifs.read((char*)&header, sizeof(HEADER));

		// If a "trainer" exists we can skip it
		if (header.mapper1 & 0x04)
			ifs.seekg(KB/2, std::ios_base::cur);

		// Determine Mapper ID
		mapper_id = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		// "Discover" File Format
		uint8_t file_type = 1;

		switch (file_type) {
			case 0: break;
			case 1:
				program_banks_n = header.prg_rom_chunks;
				program_memory.resize(program_banks_n * 16 * KB);
				ifs.read((char*)program_memory.data(), program_memory.size());

				pattern_banks_n = header.chr_rom_chunks;
				pattern_memory.resize(pattern_banks_n * 8 * KB);
				ifs.read((char*)pattern_memory.data(), pattern_memory.size());
				break;
			case 2: break;
		}

		// Load appropriate mapper
		pMapper = cartridgeMapper(program_banks_n, pattern_banks_n);
		

		//std::cout << "name: " << header.name << "\n";
		//std::cout << "prg_rom_chunks: " << (int)header.prg_rom_chunks << "\n";
		//std::cout << "chr_rom_chunks: " << (int)header.chr_rom_chunks << "\n";
		//std::cout << "mapper1: " << (int)header.mapper1 << "\n";
		//std::cout << "mapper2: " << (int)header.mapper2 << "\n";
		//std::cout << "prg_ram_size: " << (int)header.prg_ram_size << "\n";
		//std::cout << "tv_system1: " << (int)header.tv_system1 << "\n";
		//std::cout << "tv_system2: " << (int)header.tv_system2 << "\n";

		image_valid = true;
		ifs.close();
	}
}

cartridge::~cartridge() {}

bool cartridge::ImageValid() {
	return image_valid;
}

bool cartridge::cpu_read(uint16_t addr, uint8_t &data) {
	uint32_t mapped_addr = 0;
	if (pMapper.cpu_map_read(addr, mapped_addr)) {
		data = program_memory[mapped_addr];
		return true;
	}
	return false;
}

bool cartridge::cpu_write(uint16_t addr, uint8_t data) {
	uint32_t mapped_addr = 0;
	if (pMapper.cpu_map_write(addr, mapped_addr)) {
		program_memory[mapped_addr] = data;
		return true;
	}
	return false;
}

bool cartridge::ppu_read(uint16_t addr, uint8_t& data) {
	uint32_t mapped_addr = 0;
	if (pMapper.ppu_map_read(addr, mapped_addr)) {
		data = pattern_memory[mapped_addr];
		return true;
	}
	return false;
}

bool cartridge::ppu_write(uint16_t addr, uint8_t data) {
	uint32_t mapped_addr = 0;
	if (pMapper.ppu_map_read(addr, mapped_addr)) {
		pattern_memory[mapped_addr] = data;
		return true;
	}
	return false;
}
