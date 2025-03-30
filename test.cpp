#include <string>

#include <gtest/gtest.h>

#include "Cartridge.hpp"

/** Tests cartridge loading for NES with only 6502 implemented **/
// Specially, I test if PRG-ROM is loaded into the cartridge's PRG_ROM vector properly
TEST(CartridgeLoading, PRG_ROM) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms1.nes";
    cartridge.loadROM(rom_path);
}
//Tests that the program exits and outputs error if rom file is not iNES format
TEST(CartridgeLoading, InvalidFormat) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms2.nes";
    ASSERT_DEATH(cartridge.loadROM(rom_path), "Invalid NES ROM file format! Only use files of iNES format. ");
}