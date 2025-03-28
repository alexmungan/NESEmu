#ifndef NES_HPP
#define NES_HPP

#include <string>

#include "SystemBus.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"

class NES {
public:
    //TODO: constructor()
    //void initialize(const std::string &romPath);
    //void run();

private:
    SystemBus systemBus;
    //PPUBus ppuBus;
    CPU cpu;
    //PPU ppu;
    //APU apu;
    Cartridge cart;

public:
    //TODO: main RUN() function, add connect() functions too

};

#endif
