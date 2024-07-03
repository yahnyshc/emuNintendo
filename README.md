# C++ Nintendo Entertainment System Emulator
Undertaking this project has been one of the most challenging yet exciting ventures for me. It involved extensive online research and learning from experienced developers and their emulator implementations. Building a software implementation of a CPU, buses, RAM, and screen rendering is fascinating, offering deep insights into the underlying workings of computer hardware and assembly instructions.

## Features
- CPU Clock cycles are simulated and the final computation is done at the last cycle.
- CPU has status flags to store the state of the execution of the instruction. (Accumulator, Carry, Overflow, etc)
- CPU communicates with the RAM using bus class implementation. (bus.h/bus.cpp)
- Possible instructions, mnemonics, and their variations are hardcoded in the cpu class. (cpu.h/cpu.cpp)

## Getting Started
- Clone the repository
- Go to the /Scripts folder and run the appropriate Setup script.
- Run the Microsoft Visual Studio 2022 "New Project.sln" solution generated in the project root folder.
- Build and run App/Source/App.cpp 

## License
- UNLICENSE for this repository (see `UNLICENSE.txt` for more details)
- Premake is licensed under BSD 3-Clause (see included LICENSE.txt file for more details)

## Copyright
- Copyright 2018, 2019, 2020, 2021 OneLoneCoder.com
