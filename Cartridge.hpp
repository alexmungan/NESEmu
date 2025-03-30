
#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Constants.hpp"

class Cartridge {
public:
    Cartridge();
    ~Cartridge();

private:
    std::vector<uint8_t> PRG_ROM;
    std::vector<uint8_t> CHR_ROM;

public:
    //Use the filepath to the .nes file and use contents to init CHR-ROM and PRG-ROM
    void loadROM(const std::string &rom_path);

    /** Reading and writing Cartridge Memory **/
    /** Mapper chip - (converts requested address into index into some cartridge memory space **/
    //Read a value from the PRG ROM memory space
    uint8_t readPRG(uint16_t address);

    //Read a value from the CHR ROM memory space
    uint8_t readCHR(uint16_t address);

};

#endif //CARTRIDGE_HPP
