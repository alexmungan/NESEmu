#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include <vector>

#include "SystemBus.hpp"
#include "global_vars.hpp"

enum Flags {
    C, Z, I, D, B, U, V, N
};

//13 addressing modes
enum AddressingMode {
    Accum, IMM, ABS, ZP, ZPX, ZPY, ABSX, ABSY, IMP, REL, INDX, INDY, IND
};


class CPU {
    using cycle_operation = uint8_t (CPU::*)();
public:
    CPU();
    ~CPU();

    /** Registers **/
    uint8_t A = 0x00;
    uint8_t X = 0x00;
    uint8_t Y = 0x00;
    uint8_t SP = 0xFD;
    uint16_t PC = 0x00; //0xFFFC???
    uint8_t status_reg = 0b00000100; //bitfield

    //Helper functions
    //Set some bit in status register to 0 or 1
    void setStatusReg(bool set, Flags flag);
    bool getStatusReg(Flags flag);

    //TODO: opcode  //
    //uint8_t workingData = 0x00; //
    //TODO: fetchedOperand??? use 16 bit data type?
    uint8_t opcode;
    //Holds the number of cycles left to finish executing current instruction
    uint8_t instr_remaining_cycles = 0;

private:
    /** BUS connection **/
    SystemBus* system_bus_ptr = nullptr;
public:
    //Create a connection to the bus
    void connect2Bus(SystemBus *bus_ptr);
    //Read value from data bus
    uint8_t read(uint16_t address);
    //Write value to data bus
    void write(uint16_t address, uint8_t data);

private:
    /** 2A03 opcode matrix **/
    struct opInfo {
        std::string pneumonic;
        AddressingMode addressing_mode = IMP;
        std::vector<cycle_operation> cycle_op_list; //List of cycle-based-operations that make up the instruction, stored in reverse order so that (instr_cycles_remaining - 1) can be used to index into it
        uint8_t cycles = 0;
    };
    std::vector<opInfo> opMatrix;
public:
    /** Helper functions to access opMatrix lookup table**/
    std::string getPneumonic(uint8_t opcode);
    AddressingMode getAddressingMode(uint8_t opcode);
    cycle_operation getNextFunctionPtr(uint8_t opcode); //Gives the function Ptr to the next cycle-based operation to execute for the current instruction
    uint8_t getCycles(uint8_t opcode);

    /**** CPU Cycle Based Operations ****/
    /**   All functions should return void and take no params **/

    /** General cycle based operations that are used by a variety (or most) instructions **/
    //Fetches the opcode from PRG-ROM, sets class member opcode
    //Also sets instr_cycles_remaining and takes 1 cycle itself
    uint8_t fetch_opcode();

    //Wastes a cycle, used for instructions that do some operation on a cycle w/o memory access
    //therefore, they do not take a full cycle and need this function to decrement the instr_cycles_remaining counter
    uint8_t waste_cycle();

    /** Status flag related cycle based operations **/
    //Clears the carry flag of status register, takes 0 cycles
    uint8_t clear_carry();

    //

};



#endif //CPU_H
