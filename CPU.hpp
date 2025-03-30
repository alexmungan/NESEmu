#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>

#include "SystemBus.hpp"

enum Flags {
    C, Z, I, D, B, U, V, N
};

//13 addressing modes
enum AddressingMode {
    Accum, IMM, ABS, ZP, ZPX, ZPY, ABSX, ABSY, IMP, REL, INDX, INDY, IND
};

class CPU {
public:
    CPU();
    ~CPU();

    /** Registers **/
    uint8_t A = 0x00;
    uint8_t X = 0x00;
    uint8_t Y = 0x00;
    uint8_t SP = 0x00;
    uint16_t PC = 0x00; //0xFFFC???
    uint8_t status_reg = 0b00000100; //bitfield

private:
    /** Helper functions and variables **/
    //Set some bit in status register to 0 or 1
    void setStatusReg(bool set, Flags flag);
    bool getStatusReg(Flags flag);

    //TODO: opcode  //
    //uint8_t workingData = 0x00; //
    //TODO: fetchedOperand??? use 16 bit data type?

    /** BUS connection **/
    SystemBus* system_bus_ptr = nullptr;
    //Create a connection to the bus
    void connect2Bus(SystemBus *bus_ptr);
    //Read value from data bus
    uint8_t read(uint16_t address);
    //Write value to data bus
    void write(uint16_t address, uint8_t data);

    /** 2A03 opcode matrix **/
    struct opInfo {
        std::string pneumonic;
        AddressingMode addressing_mode = IMP;
        std::vector<void(*)()> cycle_op_list; //List of cycle-based-operations that make up the instruction
        uint8_t cycles = 0;
    };
    std::array<opInfo, 16*16> opMatrix;


};



#endif //CPU_H
