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
    using cycle_operation = void (CPU::*)();
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

    uint8_t opcode = 0x00;
    uint8_t operand_1byte = 0x00; //Holds operands of addressing modes: immediate, ... TODO
    uint16_t operand_2byte = 0x0000; //Holds operand of addressing modes: TODO, . Also used to store computed addresses
    uint8_t working_data = 0x00; //Holds the value that will be used by instruction after addressing mode stuff has been handles
    uint8_t curr_micro_op = 0;
    bool interrupt_poll_cycle = false; //Is set to true on the cycle where interrupt polling occurs (normally last cycle of an instruction)
    //Function pointers to the operations that are overlapped with the next instruction's fetch_opcode and potentially the next cycle as well (for ADC Abs for example)
    cycle_operation overlap_op1 = nullptr;
    cycle_operation overlap_op2 = nullptr; //TODO: Only occurs with ADC and SBC?????

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
    };
    std::vector<opInfo> opMatrix;
public:
    /** Helper functions to access opMatrix lookup table**/
    std::string getPneumonic(uint8_t opcode);
    AddressingMode getAddressingMode(uint8_t opcode);
    cycle_operation getNextFunctionPtr(uint8_t opcode); //Gives the function Ptr to the next cycle-based operation to execute for the current instruction
    size_t getListSize(uint8_t opcode); //Get size micro op list

    /**** CPU Cycle Based Operations ****/
    /**   All functions should return void and take no params **/

    /** General operations that may occur on a cycle for many instructions **/
    //Fetches the opcode from PRG-ROM, finishes previous operation
    //Sets instruction related state variables such as opcode, curr_micro_op, ... TODO
    void fetch_opcode();

    //Fetches the next byte at PRG-ROM to get the operand to the instruction
    //This is used for addressing modes: IMM, TODO
    uint8_t fetch_operand_1byte();
    //This is used for addressing modes: absolute, TODO
    uint8_t fetch_operand_2byte_LSB();
    uint8_t fetch_operand_2byte_MSB();

    //Performs dummy read
    void dummy_read();

    /** Addressing mode functions **/
    //Immediate mode: the 1 byte fetched after the opcode is the working data
    uint8_t set_working_data_immediate();

    //Zero page: the 1 byte fetched after the opcode is an address into the zero page, the value at this address is the working adata
    uint8_t set_working_data_zero_page();

    //Zero page X: the 1 bytes feteched after the opcode is an address into zp, add X to zero page address and the value at this address is working data
    uint8_t set_working_data_zero_page_X();

    //Absolute: the 2 bytes feteched after opcode form absolute address, the value found there is working data
    uint8_t set_working_data_absolute();

    //Abs X: the 2 bytes feteched after opcode form abs address and then X is added, the value at the resulting addr is working data
    //Note: the second function is only used if there is a page cross, requiring and extra cycle
    uint8_t set_working_data_absolute_X_1();
    uint8_t set_working_data_absolute_X_2();

    //Abs Y: same as abs X but Y register is used
    uint8_t set_working_data_absolute_Y_1();
    uint8_t set_working_data_absolute_Y_2();

    //Indexed indirect: the 1 byte fetched with X added to it is the address to the
    //LSB of another address, the MSB is at the next address.
    //Both combined form the address of the working_data
    //Fetch LSB
    uint8_t set_working_data_indexed_indirect_1();
    //Fetch MSB to form full address
    uint8_t set_working_data_indexed_indirect_2();
    //Get data from full address
    uint8_t set_working_data_indexed_indirect_3();

    //Indirect Indexed: the 1 byte fetched is used to get LSB
    //MSB is at the next address, combine both to get full address
    //Add Y to address, then fetch data from final computed address
    //Get LSB
    uint8_t set_working_data_indirect_indexed_1();
    //Get MSB, combine LSB and MSB
    uint8_t set_working_data_indirect_indexed_2();
    //Add Y to address and access data, if adding causes page boundary to be crossed, access happen in part 4
    uint8_t set_working_data_indirect_indexed_3();
    //Access data from final address
    uint8_t set_working_data_indirect_indexed_4();

    //
    uint8_t copy_operand1to2();

    /** Data Movement **/
    uint8_t load_A();
    uint8_t load_Y();
    uint8_t store_A();

    /** FLAG instructions **/
    //CLC
    //Cycle 1: fetch OP CODE and finish previous op (overlap_op1), PC++
    //Cycle 2: dummy_read(), decode OP CODE, poll for interrupts (on last cycle of instruction),
    //check overlap_op2 to see if previous instr still needs operation to be done
    void CLC_cycle2();
    //Cycle 3 (start of next instruction): fetch next OP CODE and finish CLC (set C <- 0), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    void clear_carry(); //This is the operation of CLC that is overlapped with the next cycle

    //CLI
    //Does the same steps as CLC except that I is set to 0 instead of C
    void CLI_cycle2();
    void clear_interrupt_disable();

    //CLV
    //Does the same steps as CLC except that V is set to 0 instead of C
    void CLV_cycle2();
    void clear_overflow();

    //CLD
    //Does the same steps as CLC except that D is set to 0 instead of C
    void CLD_cycle2();
    void clear_decimal();

    //SEC
    //Cycle 1: fetch OP CODE and finish previous op (overlap_op1), PC++
    //Cycle 2: dummy_read(), decode OP CODE, poll for interrupts (on last cycle of instruction),
    //check overlap_op2 to see if previous instr still needs operation to be done
    void SEC_cycle2();
    //Cycle 3 (start of next instruction): fetch next OP CODE and finish SEC (set C <- 1), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    void set_carry();

    //SEI
    //Does the same steps as SEC except that I is set to 1 instead of C
    void SEI_cycle2();
    void set_interrupt_disable();

    //SED
    //Does the same steps as SEC except that D is set to 1 instead of C
    void SED_cycle2();
    void set_decimal();

    /** Bitwise **/
    //Bitwise AND - takes 0 cycles
    uint8_t AND();

};



#endif //CPU_H
