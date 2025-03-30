#include <string>
#include <cstdint>

#include <gtest/gtest.h>

#include "Cartridge.hpp"
#include "Constants.hpp"

/** Cartridge Testing **/
/** With Only 6502 Implemented **/
// Specially, I test if PRG-ROM is loaded into the cartridge's PRG_ROM vector properly
// Also tests reading from PRG_ROM
TEST(CartridgeLoading, PRG_ROM) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms1.nes";
    cartridge.loadROM(rom_path);

    std::array<uint8_t, 16> test_buffer = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x00};

    for (int i = 0; i < 16 * KIBIBYTE; i++)
        ASSERT_EQ(cartridge.readPRG(PRG_ROM_START + i), test_buffer[i % 16]);
}
//Tests that the program exits and outputs error if rom file is not iNES format
TEST(CartridgeLoading, InvalidFormat) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms2.nes";
    ASSERT_DEATH(cartridge.loadROM(rom_path), "Invalid NES ROM file format! Only use files of iNES format. ");
}

/** SystemBus Testing **/
/** With Only 6502 Implemented **/