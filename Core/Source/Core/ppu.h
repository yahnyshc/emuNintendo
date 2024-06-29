#pragma once
#include <cstdint>
#include <memory>
#include "cartridge.h"

#include "olcPixelGameEngine.h"

#define KB 1024

class ppu {
public:
	ppu();
	~ppu();

public:
	uint8_t name_table[2][KB];
	uint8_t palette[32];
	// uint8_t pattern[2][4*KB];


	uint8_t cpu_read(uint16_t addr, bool read_only = false);
	void cpu_write(uint16_t addr, uint8_t data);

	uint8_t ppu_read(uint16_t addr, bool read_only = false);
	void ppu_write(uint16_t addr, uint8_t data);

public:
	void connect_cartridge(const std::shared_ptr<cartridge>& ctrg);
	void clock();

private:
	std::shared_ptr<cartridge> ctrg;

private:
	olc::Pixel  palScreen[0x40];
	olc::Sprite* sprScreen;
	olc::Sprite* sprNameTable[2];
	olc::Sprite* sprPatternTable[2];

public:
	// Debugging Utilities
	olc::Sprite& GetScreen();
	olc::Sprite& GetNameTable(uint8_t i);
	olc::Sprite& GetPatternTable(uint8_t i);
	bool frame_complete = false;

private:
	int16_t scanline = 0; // row on the screen
	int16_t cycle = 0; // which column of the screen
};
