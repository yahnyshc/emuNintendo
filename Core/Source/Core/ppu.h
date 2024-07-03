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
	uint8_t name_table[2][KB] = {};
	uint8_t palette[32] = {};
	uint8_t pattern[2][4 * KB] = {};

	bool nminterupt = false;


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
	olc::Sprite& GetPatternTable(uint8_t i, uint8_t palette);
	olc::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);
	bool frame_complete = false;

private:
	int16_t scanline = 0; // row on the screen
	int16_t cycle = 0; // which column of the screen

	union {
		struct {
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	} status;

	union {
		struct {
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};
		uint8_t reg;
	} mask;

	union PPUCTRL {
		struct {
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1; // unused
			uint8_t enable_nmi : 1;
		};

		uint8_t reg;
	} control;

	union loopy_register {
		struct {

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t reg = 0x0000;
	};

	loopy_register vram_addr;
	loopy_register tram_addr;

	uint8_t fine_x = 0x00;

	uint8_t address_latch = 0x00;
	uint8_t ppu_data_buffer = 0x00;

	uint8_t backg_next_tile_id = 0x00;
	uint8_t backg_next_tile_attrib = 0x00;
	uint8_t backg_next_tile_lsb = 0x00;
	uint8_t backg_next_tile_msb = 0x00;
	uint16_t backg_shifter_pattern_lo = 0x0000;
	uint16_t backg_shifter_pattern_hi = 0x0000;
	uint16_t backg_shifter_attrib_lo = 0x0000;
	uint16_t backg_shifter_attrib_hi = 0x0000;
};