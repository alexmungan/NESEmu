#ifndef NES_HPP
#define NES_HPP

#include <string>

#include "SystemBus.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"

class NES {
public:
    NES();
    ~NES();

private:
    SystemBus systemBus;
    //PPUBus ppuBus;
    CPU cpu;
    //PPU ppu;
    //APU apu;
    Cartridge cart;

public:
    //Function to initialize NES system (basically, insert cartridge)
    void initialize(const std::string &romPath);

    //Function to run the inserted game
    void run();

};

#endif
