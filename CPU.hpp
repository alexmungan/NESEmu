#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "SystemBus.hpp"
#include "global_vars.hpp"
#include "Constants.hpp"

enum Flags {
    C, Z, I, D, B, U, V, N
};

//13 addressing modes
enum AddressingMode {
    Accum, IMM, ABS, ZP, ZPX, ZPY, ABSX, ABSY, IMP, REL, INDX, INDY, IND, INTERRUPT
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
    uint16_t PC = 0x0000;
    uint8_t status_reg = 0b00000100; //bitfield

    /** Interrupt poll status **/
    //Holds whether the interrupt signals were detected last cycle
    bool IRQ;
    bool NMI;
    //Holds whether the interrupt signals (for each type) have occurred that cycle
    bool RESET_signal;
    bool IRQ_signal;
    bool NMI_signal;

    //Helper functions
    //Set some bit in status register to 0 or 1
    void setStatusReg(bool set, Flags flag);
    bool getStatusReg(Flags flag);

    uint16_t opcode = 0x00; //Is 16 bits rather than 8 b/c I added extra "opcodes" to the instr table for interrupt sequences
    uint8_t operand_byte1 = 0x00; //Holds the first byte after opcode
    uint8_t operand_byte2 = 0x0000; //Holds the 2nd byte after opcode
    uint8_t working_data = 0x00; //Holds the value that will be used by instruction after addressing mode stuff has been handled, it may also hold an ALU result (for ADC instr for ex) but we must be careful not to modify working_data before storing this ALU result to A
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
    /** Helper functions for logging and stepMode **/
    //Outputs CPU state to terminal
    void cpu_dump();

    //Converts integer representation of addressing mode to string representation
    std::string addressingModeToString(AddressingMode addressing_mode);

    /** Helper functions to access opMatrix lookup table**/
    std::string getPneumonic(uint16_t opcode);
    AddressingMode getAddressingMode(uint16_t opcode);
    cycle_operation getNextFunctionPtr(uint16_t opcode); //Gives the function Ptr to the next cycle-based operation to execute for the current instruction
    size_t getListSize(uint16_t opcode); //Get size micro op list

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

    //Wastes a cycle doing nothing - only used during RESET interrupt sequence b/c some of those cycles do unimportant stuff
    //Gonna set the I flag here, again it doesn't really matter which cycle we do it during the sequence since interrupt polling is not done during interrupt sequnces
    //SP value doesn't matter as the programmer should set it in their handler
    void waste_cycle();

    /** Addressing mode functions **/
    //Cycle 2 fetches the next instruction byte into operand_byte1 and increments PC
    void ABS_cycle2();

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

    /** Data Movement (access ops)**/
    //LDA IMM
    //Cycle 1: fetch OP CODE and finish previous op (overlap_op1), PC++
    //Cycle 2: fetch next instr byte, decode OP CODE, PC++
    //check overlap_op2 to see if prev instr still needs internal operation to be done
    void LDA_IMM_cycle2();
    //Cycle 3 (start of next instr): fetch next OP CODE and finish LDA IMM (A <- IMM val), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    void load_A(); //This is the operation that is overlapped with cycle 3

    //LDX IMM
    //Same as LDA IMM but for X reg
    void LDX_IMM_cycle2();
    void load_X();

    //LDY IMM
    //Same as LDA IMM but for Y reg
    void LDY_IMM_cycle2();
    void load_Y();

    /** Data Movement (transfer ops) **/
    //TAX
    //Cycle 1: fetch OP CODE and finish prev op (overlap_op1), PC++
    //fetch_opcode()
    //Cycle 2: dummy_read(), decode OP CODE, poll for interrupts (on last cycle of instr),
    //check overlap2 to see if prev instr still needs operation done
    void TAX_cycle2();
    //Cycle 3 (start of next isntruction): fetch next OP CODE and finish TAX (X <- A, set Z and N flags), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    //load_X()

    //TXA
    //Same as TAX except transfer direction is switched
    void TXA_cycle2();

    //TAY
    //Same as TAX except Y used in place of X
    void TAY_cycle2();

    //TYA
    //Same as TAY except transfer direction is switched
    void TYA_cycle2();

    //TSX
    //Same as TAX except SP is used in place of A
    void TSX_cycle2();

    //TXS
    //Same as TSX but transfer direction is switched and N and Z flags are not set
    void TXS_cycle2();
    //Helper
    void load_SP();


    /** FLAG instructions **/
    //CLC
    //Cycle 1: fetch OP CODE and finish previous op (overlap_op1), PC++
    //Cycle 2: dummy_read(), decode OP CODE, poll for interrupts (on last cycle of instruction),
    //check overlap_op2 to see if previous instr still needs operation to be done
    void CLC_cycle2();
    //Cycle 3 (start of next instruction): fetch next OP CODE and finish CLC (set C <- 0), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    void clear_carry(); //This is the operation of CLC that is overlapped with cycle 3

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

    /** Jump Instructions **/
    //JMP Abs
    //Cycle 1: fetch OP CODE and finish prev op (overlap_op1), PC++
    //Cycle 2: fetch adl, decode OP CODE, PC++, check overlap_op2
    //void ABS_cycle2();
    //Cycle 3: fetch adh, PC <-- adh, adl, poll for interrupts
    void JMP_ABS_cycle3();
    //Cycle 4 (start of next instr): fetch next OP CODE, no overlapped op, PC++

    /** Interrupt Sequences **/
    //RESET
    //Cycles 1-5: don't really matter it seems
    //Cycle 6: Fetch address of RESET handler
    void fetch_RESET_vector_LSB();
    //Cycle 7: Fetch address of RESET handler
    void fetch_RESET_vector_MSB();

};



#endif //CPU_H
