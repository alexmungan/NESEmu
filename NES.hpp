#ifndef NES_HPP
#define NES_HPP

#include <string>

#include "SystemBus.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"
#include "global_vars.hpp"
#include "Constants.hpp"

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
    Cartridge cartridge;

public:
    //Function to initialize NES system (basically, insert cartridge)
    void initialize(const std::string &rom_path);

    //Function to run the inserted game
    void run();

};

#endif
