#ifndef NES_HPP
#define NES_HPP

#include <string>

#include "Bus.hpp"
#include "CPU.hpp"

class NES {
public:
    //TODO: constructor()
    //void initialize(const std::string &romPath);
    //void run();

private:
    Bus bus;
    CPU cpu;
    //PPU ppu;
    //APU apu;
    //Memory memory;
    //Cartridge cartridge;
};

#endif
