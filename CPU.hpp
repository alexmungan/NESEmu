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
    explicit CPU(Bus& bus_ptr);
    ~CPU();

    /** Registers **/
    uint8_t A = 0x00;
    uint8_t X = 0x00;
    uint8_t Y = 0x00;
    uint8_t SP = 0x00;
    uint16_t PC = 0x00;
    uint8_t status_reg = 0x00; //bitfield

private:
    /** Helper functions and variables **/
    //Set some bit in status register to 0 or 1
    void setStatusReg(bool set, Flags flag);
    bool getStatusReg(Flags flag);

    //Working data
    uint8_t instrData = 0x00;
    //Function to get instruction's data operand
    //Various possible sources based on addressing mode
    uint8_t getInstrData();

    /** BUS connection **/
    SystemBus* bus;
    //Read value from data bus
    uint8_t read(uint16_t address);
    //Write value to data bus
    void write(uint16_t address, uint8_t data);

    /** 2A03 instruction matrix **/
    struct opInfo {
        std::string pneumonic;
        AddressingMode addressing_mode = IMP;
        //TODO:Add pointer to addressing mode function
        //TODO:Add pointer to execution function
        uint8_t cycles = 0;
    };
    std::array<opInfo, 16*16> instrMatrix;

    /** CPU Addressing Modes **/

    /** CPU instructions **/

    /** CPU main emulation function **/


};



#endif //CPU_H
