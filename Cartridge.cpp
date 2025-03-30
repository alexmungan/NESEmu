#include "Cartridge.hpp"

Cartridge::Cartridge() {
    PRG_ROM.reserve(8* KIBIBYTE);
    CHR_ROM.reserve(8 * KIBIBYTE);
}

Cartridge::~Cartridge() {
}

void Cartridge::loadROM(const std::string &rom_path) {
    std::vector<uint8_t> rom_image;

    //Get file handle to .nes file
    std::ifstream rom_file(rom_path, std::ios::binary | std::ios::ate);
    if (!rom_file) {
        std::cerr << "Error opening file: " << rom_path << std::endl;
        //TODO:cleanup resources before exit
        exit(EXIT_FAILURE);
    }

    std::streamsize file_size = rom_file.tellg();
    rom_file.seekg(0, std::ios::beg); //Go back to beginning of file

    // Read the file into buffer
    rom_image.resize(file_size);
    if (!rom_file.read(reinterpret_cast<char*>(rom_image.data()), file_size)) {
        std::cerr << "Error reading file: " << rom_path << std::endl;
        //TODO:cleanup resources before exit
        exit(EXIT_FAILURE);
    }
    if (rom_file.is_open()) rom_file.close();

    //Check File Format (Using original .nes format for now)
    bool iNESFormat=false;
    if (rom_image[0]=='N' && rom_image[1]=='E' && rom_image[2]=='S' && rom_image[3]==0x1A)
        iNESFormat=true;

    /*
    bool NES20Format=false;
    if (iNESFormat==true && (rom_image[7]&0x0C)==0x08)
        NES20Format=true;
        */

    if ((!iNESFormat) /*&& (!NES20Format)*/) {
        std::cerr << "Invalid NES ROM file format! Only use files of iNES format. " << std::endl;
        //TODO:cleanup resources before exit
        exit(EXIT_FAILURE);
    }

    //At byte 4 of .nes, get PRG-ROM size (in units of 16 kiB) and converts to bytes
    size_t PRG_ROM_size = rom_image[4] * 16 * KIBIBYTE;
    PRG_ROM.resize(PRG_ROM_size);
    //Load PRG-ROM into buffer
    if (((rom_image[6] & 0x04) >> 2) == 1) { //Check if there is trainer in ROM file
        for (int i = 0; i < PRG_ROM_size; i++)
            PRG_ROM[i] = rom_image[HEADER_SIZE + TRAINER_SIZE + i];
    } else {
        for (int i = 0; i < PRG_ROM_size; i++)
            PRG_ROM[i] = rom_image[HEADER_SIZE + i];
    }


    //TODO: check other flags and meta data needed for future features

}

uint8_t Cartridge::readPRG(uint16_t address) {
    return PRG_ROM[address - PRG_ROM_START];
}

uint8_t Cartridge::readCHR(uint16_t address) {

}