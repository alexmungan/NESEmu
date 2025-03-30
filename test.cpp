#include <string>
#include <cstdint>

#include <gtest/gtest.h>

#include "Cartridge.hpp"
#include "Constants.hpp"
#include "SystemBus.hpp"

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
/** With Only 6502, system RAM, and cartridge PRG-ROM Implemented **/
//Tests that requesting an address within PRG-ROM address range works
//(Also, tests connecting a cartridge to the BUS)
TEST(SystemBus, readPRG) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms1.nes"; //PRG-ROM starts with 0x12, 0x34 ...
    cartridge.loadROM(rom_path);

    SystemBus systemBus;
    systemBus.connect2cartridge(&cartridge);

    std::array<uint8_t, 16> test_buffer = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x00};

    for (int i = 0; i < 16 * KIBIBYTE; i++)
        ASSERT_EQ(systemBus.read(PRG_ROM_START + i), test_buffer[i % 16]);
}

//Tests that attempting to write to PRG-ROM has no effect
TEST(SystemBus, writePRG) {
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms1.nes"; //PRG-ROM starts with 0x12, 0x34 ...
    cartridge.loadROM(rom_path);

    SystemBus systemBus;
    systemBus.connect2cartridge(&cartridge);

    //Attempt to write to PRG-ROM, expect that it does not change original value of 0x12
    systemBus.write(PRG_ROM_START, 0x00);
    ASSERT_EQ(systemBus.read(PRG_ROM_START), 0x12);
}

//Tests that requesting an address (to write or read) within RAM and Mirrored RAM works
TEST(SytemBus, ReadAndWriteRAM) {
    SystemBus systemBus;
    systemBus.write(0x0000, 0x12);
    systemBus.write(0x0801, 0x34); //In mirror RAM
    ASSERT_EQ(systemBus.read(0x0000), 0x12);
    ASSERT_EQ(systemBus.read(0x0801), 0x34); //In mirror RAM
}

/** CPU (Ricoh 6502) Testing **/
/** CPU only, no apu, ppu. Mem space is only RAM and PRG-ROM **/
