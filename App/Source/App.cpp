/*
	olc::NES - Part #3 - Buses, Rams, Roms & Mappers
	"Thanks Dad for believing computers were gonna be a big deal..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	Relevant Video: https://youtu.be/xdzOvpYPmGE

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, �OneLoneCoder 2019
*/

#include <iostream>
#include <sstream>
#include <direct.h>

#include "Core/bus.h"
#include "Core/cpu.h"

#define OLC_PGE_APPLICATION
#include "Core/olcPixelGameEngine.h"



class Demo : public olc::PixelGameEngine
{
public:
	Demo() { sAppName = "olc2C02 Demonstration"; }

private:
	// The NES
	bus nes;
	std::shared_ptr<cartridge> cart;
	bool bEmulationRun = false;
	float fResidualTime = 0.0f;

	uint8_t nSelectedPalette = 0x00;


private:
	// Support Utilities
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++)
		{
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(nes.cpu_read(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", nes.olc6502.status & cpu::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", nes.olc6502.status & cpu::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", nes.olc6502.status & cpu::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", nes.olc6502.status & cpu::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", nes.olc6502.status & cpu::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", nes.olc6502.status & cpu::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", nes.olc6502.status & cpu::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", nes.olc6502.status & cpu::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(nes.olc6502.pc, 4));
		DrawString(x, y + 20, "A: $" + hex(nes.olc6502.a, 2) + "  [" + std::to_string(nes.olc6502.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(nes.olc6502.x, 2) + "  [" + std::to_string(nes.olc6502.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(nes.olc6502.y, 2) + "  [" + std::to_string(nes.olc6502.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes.olc6502.stack_p, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(nes.olc6502.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(nes.olc6502.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		// Load the cartridge
		
		cart = std::make_shared<cartridge>("ic.nes");
		
		if (!cart->ImageValid()) {
			return false;
		}

		// Insert into NES
		nes.insert_cartridge(cart);

		// Extract dissassembly
		mapAsm = nes.olc6502.disassemble(0x0000, 0xFFFF);

		// Reset NES
		nes.reset();
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);



		if (bEmulationRun)
		{
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { nes.clock(); } while (!nes.olc2C02.frame_complete);
				nes.olc2C02.frame_complete = false;
			}
		}
		else
		{
			// Emulate code step-by-step
			if (GetKey(olc::Key::C).bPressed)
			{
				// Clock enough times to execute a whole CPU instruction
				do { nes.clock(); } while (!nes.olc6502.complete());
				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain
				// those out
				do { nes.clock(); } while (nes.olc6502.complete());
			}

			// Emulate one whole frame
			if (GetKey(olc::Key::F).bPressed)
			{
				// Clock enough times to draw a single frame
				do { nes.clock(); } while (!nes.olc2C02.frame_complete);
				// Use residual clock cycles to complete current instruction
				do { nes.clock(); } while (!nes.olc6502.complete());
				// Reset frame completion flag
				nes.olc2C02.frame_complete = false;
			}
		}


		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::R).bPressed) nes.reset();
		if (GetKey(olc::Key::P).bPressed) (++nSelectedPalette) &= 0x07;

		DrawCpu(516, 2);
		DrawCode(516, 72, 26);

		// Draw Palettes & Pattern Tables ==============================================
		const int nSwatchSize = 6;
		for (int p = 0; p < 8; p++) // For each palette
			for (int s = 0; s < 4; s++) // For each index
				FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
					nSwatchSize, nSwatchSize, nes.olc2C02.GetColourFromPaletteRam(p, s));

		// Draw selection reticule around selected palette
		DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

		// Generate Pattern Tables
		DrawSprite(516, 348, &nes.olc2C02.GetPatternTable(0, nSelectedPalette));
		DrawSprite(648, 348, &nes.olc2C02.GetPatternTable(1, nSelectedPalette));

		DrawSprite(0, 0, &nes.olc2C02.GetScreen(), 2);

		//olc::Sprite& s = nes.olc2C02.GetPatternTable(1, nSelectedPalette);
		//for (uint8_t y = 0; y < 30; y++)
		//	for (uint8_t x = 0; x < 32; x++) {
		//		//DrawString(x * 16, y * 16, hex((uint32_t)nes.olc2C02.name_table[0][y * 32 + x], 2));
		//		uint8_t id = (uint32_t)nes.olc2C02.name_table[0][y * 32 + x];
		//		DrawPartialSprite(x * 16, y * 16, &s, 
		//			(id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 2);
		//	}
				
		
		return true;
	}
};





int main()
{
	Demo demo;
	demo.Construct(780, 480, 2, 2);
	demo.Start();
	return 0;
}