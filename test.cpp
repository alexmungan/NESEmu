#include <string>
#include <cstdint>

#include <gtest/gtest.h>

#include "Cartridge.hpp"
#include "Constants.hpp"
#include "SystemBus.hpp"
#include "CPU.hpp"

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
TEST(CPU, SetAndGetStatusReg) {
    CPU cpu;

    cpu.setStatusReg(true, C);
    cpu.setStatusReg(true, Z);
    cpu.setStatusReg(true, I);
    cpu.setStatusReg(true, D);
    cpu.setStatusReg(true, B);
    cpu.setStatusReg(true, U);
    cpu.setStatusReg(true, V);
    cpu.setStatusReg(true, N);

    EXPECT_EQ(cpu.getStatusReg(C), true);
    EXPECT_EQ(cpu.getStatusReg(Z), true);
    EXPECT_EQ(cpu.getStatusReg(I), true);
    EXPECT_EQ(cpu.getStatusReg(D), true);
    EXPECT_EQ(cpu.getStatusReg(B), true);
    EXPECT_EQ(cpu.getStatusReg(U), true);
    EXPECT_EQ(cpu.getStatusReg(V), true);
    EXPECT_EQ(cpu.getStatusReg(N), true);

    cpu.setStatusReg(false, Z);
    EXPECT_EQ(cpu.getStatusReg(Z), false);
}
//Test connecting to the bus and reading and writing RAM (also retests RAM mirroring)
TEST(CPU, BusReadAndWriteRAM) {
    CPU cpu;
    SystemBus bus;
    cpu.connect2Bus(&bus);

    cpu.write(0x0000, 0x00);
    cpu.write(0x0001, 0xFC);
    cpu.write(0x0802, 0x13);

    EXPECT_EQ(cpu.read(0x0000), 0x00);
    EXPECT_EQ(cpu.read(0x0800), 0x00);
    EXPECT_EQ(cpu.read(0x0001), 0xFC);
    EXPECT_EQ(cpu.read(0x0801), 0xFC);
    EXPECT_EQ(cpu.read(0x0002), 0x13);
    EXPECT_EQ(cpu.read(0x0802), 0x13);

}

//Test reading (and failed writing) to cartridge PRG-ROM from CPU
TEST(CPU, BusReadAndWritePRG) {
    CPU cpu;
    SystemBus bus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_loadRoms1.nes"; //PRG-ROM starts with 0x12, 0x34 ...
    cartridge.loadROM(rom_path);
    bus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&bus);

    cpu.write(0x08000, 0x00);
    cpu.write(0x8001, 0xFC);
    cpu.write(0x8002, 0x13);

    EXPECT_EQ(cpu.read(0x8000), 0x12);
    EXPECT_EQ(cpu.read(0x8001), 0x34);
    EXPECT_EQ(cpu.read(0x8002), 0x56);

}

//Test getPneumonic from opcode matrix
TEST(CPU, getPneumonic) {
    CPU cpu;
    ASSERT_EQ(cpu.getPneumonic(0x18), "CLC");
}

TEST(CPU, getAddressingMode) {
    CPU cpu;
    ASSERT_EQ(cpu.getAddressingMode(0x18), IMP);
}

TEST(CPU, getNextFunctionPtr) {
    CPU cpu;
    cpu.opcode = 0x18;
    ASSERT_TRUE(cpu.getNextFunctionPtr(cpu.opcode) == &CPU::clear_carry);
}

TEST(CPU, getCycles) {
    CPU cpu;
    cpu.opcode = 0x18;
    ASSERT_EQ(cpu.getCycles(cpu.opcode), 2);
}

TEST(CPU, getListSize) {
    CPU cpu;
    cpu.opcode = 0x18;
    ASSERT_EQ(cpu.getListSize(cpu.opcode), 2);
}

TEST(CPU, fetch_opcode) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_CLC.nes"; //PRG-ROM contains 0x18 opcode
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x18);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);
    ASSERT_EQ(cpu.cycle_op_list_size,2);
}

TEST(CPU, waste_cycle) {
    CPU cpu;
    cpu.instr_remaining_cycles = 1;
    cpu.curr_micro_op = 1;
    ASSERT_EQ(cpu.waste_cycle(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.curr_micro_op, 2);
}

TEST(CPU, clear_cary) {
    CPU cpu;
    cpu.status_reg = 0b11111111;
    cpu.curr_micro_op = 0;
    ASSERT_EQ(cpu.clear_carry(), 0);
    ASSERT_EQ(cpu.getStatusReg(C), 0);
    ASSERT_EQ(cpu.curr_micro_op, 1);
}

//TEST CLC instruction
TEST(CPU, CLC) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_CLC.nes"; //PRG-ROM contains 0x18 opcode
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    cpu.status_reg = 0b11111111;

    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x18);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }

    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(C), 0);
}

//tests CLI CLV and CLD
//Also tests that executing multiple instructions one after the other works as expected
TEST(CPU, CLICLVCLD) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_clear.nes"; //PRG-ROM contains 0x58 0xB8 0xD8 opcodes
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    cpu.status_reg = 0b11111111;

    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x58);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(I), 0);

    ASSERT_EQ(cpu.PC, 0x8001);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0xB8);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(V), 0);

    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0xD8);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8003);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(D), 0);
}

//Test SEC, SED, SEI
TEST(CPU, SET) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_set.nes"; //PRG-ROM contains 0x38 0xF8 0x78 opcodes
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    cpu.status_reg = 0b00000000;

    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x38);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(C), 1);

    ASSERT_EQ(cpu.PC, 0x8001);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0xF8);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(D), 1);

    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x78);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8003);
    while (cpu.curr_micro_op < cpu.cycle_op_list_size) {
        auto function = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*function)();
    }
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.getStatusReg(I), 1);
}

//Test AND IMM instruction, result is not zero and is negative
TEST(CPU, AND_IMM_NZ_N) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_IMM1.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x29);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.operand_1byte, 0xF0);

    //set_working_data_immediate()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

//Test AND instruction, result is zero and is positive
TEST(CPU, AND_IMM_Z_P) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_IMM2.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x29);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.operand_1byte, 0x22);

    //set_working_data_immediate()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.working_data, 0x22);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x00);

    ASSERT_EQ(cpu.getStatusReg(Z), true);
    ASSERT_EQ(cpu.getStatusReg(N), false);
}

//Test AND zero page instruction, result is not zero and is negative
TEST(CPU, AND_ZP_NZ_N) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ZP1.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.write(0x04, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x25);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.operand_1byte, 0x04);

    //set_working_data_zero_page()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

//Test AND zero page instruction, result is zero and is positive
TEST(CPU, AND_ZP_Z_P) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ZP1.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.write(0x04, 0x22);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x25);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.operand_1byte, 0x04);

    //set_working_data_zero_page()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0x22);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x00);

    ASSERT_EQ(cpu.getStatusReg(Z), true);
    ASSERT_EQ(cpu.getStatusReg(N), false);
}

//Test AND zero page x where result is non zero and negative
TEST(CPU, AND_ZP_X_NZ_N) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ZPX1.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.X = 0x04;
    cpu.write(0x04 + 0x04, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x35);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_1byte, 0x04);

    //set_working_data_zero_page_X()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);

    //waste_cycle()
    ASSERT_EQ(cpu.curr_micro_op, 3);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
}

//AND with absolute addressing
TEST(AND, ABS) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ABS.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.write(0x0123, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x2D);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_2byte_LSB()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_2byte, 0x23);

    //fetch_operand_2byte_MSB()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8003);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.operand_2byte, 0x0123);

    //set_working_data_absolute()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 3);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

//AND Abs X WITH page crossing 0x00FF + 0x01 = 0x0100
TEST(AND, ABSX_cross) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ABSX_cross.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.X = 0x01;
    cpu.write(0x0100, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x3D);
    ASSERT_EQ(cpu.instr_remaining_cycles, 4);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_2byte_LSB()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.operand_2byte, 0xFF);

    //fetch_operand_2byte_MSB()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8003);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_2byte, 0x00FF);

    //set_working_data_absolute_X_1
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.operand_2byte, 0x0100); //already set

    //set_working_data_absolute_X_2
    ASSERT_EQ(cpu.curr_micro_op, 3);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 4);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

//AND Abs X (NO page crossing) 0x0004 + 0x04 = 0x0008
TEST(AND, ABSX) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_ABSX.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.X = 0x04;
    cpu.write(0x0008, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.PC, 0x8000);
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x3D);
    ASSERT_EQ(cpu.instr_remaining_cycles, 4);
    ASSERT_EQ(cpu.PC, 0x8001);

    //fetch_operand_2byte_LSB()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.operand_2byte, 0x0004);

    //fetch_operand_2byte_MSB()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.PC, 0x8003);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_2byte, 0x0004);

    //set_working_data_absolute_X_1
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    //internally, addr_abs set to 0x0008
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND()
    ASSERT_EQ(cpu.curr_micro_op, 4);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

//Indexed Indirect
TEST(AND, INDX) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_INDX.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.X = 0x04;
    cpu.write(0x0008, 0x0A); //LSB
    cpu.write(0x0009, 0x00); //MSB
    cpu.write(0x000A, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x21);
    ASSERT_EQ(cpu.instr_remaining_cycles, 5);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 4);
    ASSERT_EQ(cpu.operand_1byte, 0x04);

    //set_working_data_indexed_indirect_1()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.operand_1byte, 0x08);
    ASSERT_EQ(cpu.operand_2byte, 0x000A);

    //set_working_data_indexed_indirect_2()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_2byte, 0x000A);

    //set_working_data_indexed_indirect_3()
    ASSERT_EQ(cpu.curr_micro_op, 3);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND
    ASSERT_EQ(cpu.curr_micro_op, 4);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);

    //waste_cycle
    ASSERT_EQ(cpu.curr_micro_op, 5);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
}

//Indirect Indexed page not crossed
TEST(AND, INDY) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_AND_INDY.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //Initialize mock test data
    cpu.A = 0x88;
    cpu.Y = 0x04;
    cpu.write(0x0004, 0x0A); //LSB
    cpu.write(0x0005, 0x00); //MSB
    cpu.write(0x000E, 0xF0);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0x31);
    ASSERT_EQ(cpu.instr_remaining_cycles, 5);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 4);
    ASSERT_EQ(cpu.operand_1byte, 0x04);

    //set_working_data_indirect_indexed_1()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 3);
    ASSERT_EQ(cpu.operand_2byte, 0x000A);

    //set_working_data_indirect_indexed_2()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);
    ASSERT_EQ(cpu.operand_2byte, 0x000A);

    //set_working_data_indirect_3_indexed()
    ASSERT_EQ(cpu.curr_micro_op, 3);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF0);

    //AND
    ASSERT_EQ(cpu.curr_micro_op, 5);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.A, 0x80);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);

}

TEST(CPU, LDA_IMM) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_LDA_IMM.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0xA9);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.operand_1byte, 0xF5);

    //set_working_data_immediate()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF5);

    //load_A()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.A, 0xF5);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}

TEST(CPU, LDA_ZP) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_LDA_ZP.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);

    //set up mock environment
    cpu.write(0x0008, 0xF5);

    //fetch_opcode()
    cpu.PC = 0x8000;
    ASSERT_EQ(cpu.fetch_opcode(), 1);
    ASSERT_EQ(cpu.opcode, 0xA5);
    ASSERT_EQ(cpu.instr_remaining_cycles, 2);

    //fetch_operand_1byte()
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 1);
    ASSERT_EQ(cpu.operand_1byte, 0x08);

    //set_working_data_zero_page()
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 1);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.working_data, 0xF5);

    //load_A()
    ASSERT_EQ(cpu.curr_micro_op, 2);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    ASSERT_EQ((cpu.*function)(), 0);
    ASSERT_EQ(cpu.instr_remaining_cycles, 0);
    ASSERT_EQ(cpu.A, 0xF5);

    ASSERT_EQ(cpu.getStatusReg(Z), false);
    ASSERT_EQ(cpu.getStatusReg(N), true);
}


