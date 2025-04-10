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

TEST(CPU, getListSize) {
    CPU cpu;
    cpu.opcode = 0x18;
    ASSERT_EQ(cpu.getListSize(cpu.opcode), 2);
}

//Tests all of the flags instructions
//Also tests fetch_opcode, dummy_read, pipelining (i.e. executing overlap_op1 and 2), and helper functions
TEST(CPU, flags) {
    CPU cpu;
    SystemBus systemBus;
    Cartridge cartridge;
    std::string rom_path = "../ROMs/test_flags.nes";
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);
    //std::cout << "HERE1" << std::endl;

    //TEST Environment - init all status flags to 0
    cpu.status_reg = 0x00;
    cpu.setStatusReg(true, V); //Set to true here since there is no SEV instr and we can still test CLV

    cpu.PC = 0x8000;
    /* Set Flags to 1 */
    //SEC cycle 1
    cpu.fetch_opcode(); //Initial fetch to start sequence
    ASSERT_EQ(cpu.PC, 0x8001);
    ASSERT_EQ(cpu.opcode, 0x38);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    //SEC cycle 2
    //std::cout << "HERE2" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    auto function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8001);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(C), false); //Should still be false
    //SEI cycle 1
    //std::cout << "HERE3" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1); //micro_op in SEC's instr_list
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.opcode, 0x78);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(C), true); //Now it should be true
    //SEI cycle 2
    //std::cout << "HERE4" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8002);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(I), false); //Should still be false
    //SED cycle 1
    //std::cout << "HERE5" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8003);
    ASSERT_EQ(cpu.opcode, 0xFD);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(I), true); //Now it should be true
    //SED cycle 2
    //std::cout << "HERE6" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8003);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(D), false); //Should still be false

    /* Clear Flags to 0 */
    //CLC cycle 1
    //std::cout << "HERE7" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1); //micro_op in SED instr_list
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8004);
    ASSERT_EQ(cpu.opcode, 0x18);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(D), true); //Should be set now by overlap
    //CLC cycle 2
    //std::cout << "HERE8" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8004);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(C), true); //Should still be true
    //CLI cycle 1
    //std::cout << "HERE9" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8005);
    ASSERT_EQ(cpu.opcode, 0x58);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(C), false); //Now, it should be cleared
    //CLI cycle 2
    //std::cout << "HERE10" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8005);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(I), true); //Should still be true
    //CLD cycle 1
    //std::cout << "HERE11" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8006);
    ASSERT_EQ(cpu.opcode, 0xD8);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(I), false); //Now, it should be cleared
    //CLD cycle 2
    //std::cout << "HERE12" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8006);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(D), true); //Should still be true
    //CLV cycle 1
    //std::cout << "HERE13" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 1);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8007);
    ASSERT_EQ(cpu.opcode, 0xB8);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(D), false); //Now, it should be cleared
    //CLV cycle 2
    //std::cout << "HERE14" << std::endl;
    ASSERT_EQ(cpu.curr_micro_op, 0);
    function = cpu.getNextFunctionPtr(cpu.opcode);
    (cpu.*function)();
    ASSERT_EQ(cpu.PC, 0x8007);
    ASSERT_EQ(cpu.interrupt_poll_cycle, true);
    ASSERT_EQ(cpu.getStatusReg(V), true); //Should still be true
    //Do fetch_opcode so that we can see than V is cleared this cycle
    cpu.fetch_opcode(); //This will be a random byte
    ASSERT_EQ(cpu.PC, 0x8008);
    ASSERT_EQ(cpu.interrupt_poll_cycle, false);
    ASSERT_EQ(cpu.getStatusReg(V), false); //Now, it should be cleared

}



/*
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
} */


