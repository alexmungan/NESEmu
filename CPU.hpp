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
    bool IRQ = false;
    bool NMI = false;
    //Holds whether the interrupt signals (for each type) have occurred that cycle
    bool RESET_signal = false;
    bool IRQ_signal = false;
    bool NMI_signal = false;

    //Helper functions
    //Set some bit in status register to 0 or 1
    void setStatusReg(bool set, Flags flag);
    bool getStatusReg(Flags flag);

    uint16_t opcode = 0x00; //Is 16 bits rather than 8 b/c I added extra "opcodes" to the instr table for interrupt sequences
    uint16_t addr1 = 0x00;
    uint16_t addr2 = 0x00; //Used for addressing modes w/ extra layer of indirection
    uint8_t working_data = 0x00; //Holds the value that will be used by instruction after addressing mode stuff has been handled, it may also hold an ALU result (for ADC instr for ex) but we must be careful not to modify working_data before storing this ALU result to A
    uint8_t ALU_result = 0x00;
    uint8_t curr_micro_op = 0;
    bool page_crossed = false; //Used in write and RMW instructions, read instructions handle things differently
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
        std::string pneumonic = "???";
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

    //Performs dummy read
    void dummy_read();
    void dummy_read(uint16_t address);

    //Wastes a cycle doing nothing - only used during RESET interrupt sequence b/c some of those cycles do unimportant stuff
    //Gonna set the I flag here, again it doesn't really matter which cycle we do it during the sequence since interrupt polling is not done during interrupt sequnces
    //SP value doesn't matter as the programmer should set it in their handler
    void waste_cycle();

    /** Addressing mode functions **/
    //Cycle 2 fetches the next instruction byte (which is the lsb of an address), PC++
    void fetch_adl_cycle2();
    //Cycle 3 fetches the next instruction byte (which is the msb of an address), PC++
    void fetch_adh_cycle3();

    //Cycle 3 takes a ZP addr fetched during cycle 2 and adds X to it, dummy read occurs
    void ZP_X_cycle3();
    void ZP_Y_cycle3();

    //Cycle 3 fetches the next instr byte (msb of an address), while adding X to lsb, PC++
    //Notice: prefix read is to signify this is for read instructions
    void read_ABS_X_cycle3();
    void read_ABS_Y_cycle3();
    //For write instructions
    void write_ABS_X_cycle3();
    void write_ABS_Y_cycle3();

    //Might occur on different cycles for different addressing modes that page crossing may occur
    //However, it never occurs on cycle 2 (where overlap_op2 is checked) or the last cycle, where interrupts are polled
    void read_page_crossed_cycle();
    void write_page_crossed_cycle(); //similar to above, but for write and RMW instructions

    //Cycle 4 uses ZP+X indirect address to access mem and obtain lsb of address to final data
    void IND_X_cycle4();
    //Cycle 5 uses ZP+X+1 ind address to access mem and obtain msb of address to final data
    void IND_X_cycle5();

    //Cycle 3: uses ZP indirect address to access mem and obtain lsb of BASE address (Y will be added to the base in a later cycle)
    void IND_Y_cycle3();
    //Cycle 4: uses ZP+1 indirect address to access mem and obtain msb of BASE address (which may have a carry added to it if a page cross occurs when adding Y to lsb of BASE)
    void read_IND_Y_cycle4();
    //For write instructions
    void write_IND_Y_cycle4();

    //The read data cycle for RMW instructions
    //Once the final address to access the data for the instruction has been loaded/computer, this cycle fetches the data
    void RMW_read_cycle();

    /** Data Movement (load ops)**/
    //LDA IMM
    //Cycle 1: fetch OP CODE and finish previous op (overlap_op1), PC++
    //Cycle 2: fetch next instr byte (imm value), decode OP CODE, PC++
    //check overlap_op2 to see if prev instr still needs internal operation to be done
    void LDA_IMM_cycle2();
    //Cycle 3 (start of next instr): fetch next OP CODE and finish LDA IMM (A <- IMM val), PC++
    //This cycle 3 could also be the first cycle of an interrupt sequence
    void load_A(); //This is the operation that is overlapped with cycle 3

    //LDA ZP
    //Cycle 1: fetch OP CODE and finish prev op (overlap_op1), PC++
    //fetch_opcode()
    //Cycle 2: fetch next instr byte (ZP addr), decode OP CODE, PC++
    //void fetch_adl_cycle2()
    //check overlap_op2 to see if prev instr still needs internal operation to be done
    //Cycle 3: fetch data from ZP addr, poll for interrupts
    void LDA_fetch_data();
    //Cycle 4(start of next instr): fetch next OP CODE and finish LDA ZP (A <- data), PC++
    //This cycle 4 could also be the first cycle of an interrupt sequence

    //LDA ZP, X
    //cycle 1: fetch_opcode()
    //cycle 2: fetch_adl_cycle2()
    //cycle 3: dummy read, add X to the fetched ZP addr in cycle 2
    //void ZP_X_cycle3()
    //cycle 4: fetch data from ZP+X, poll for interrupts
    //void LDA_ZP_fetch_data();
    //Cycle 5 (start of next instr): fetch next OP CODE and finish LDA ZP,X (A <- data), flags set, PC++

    //LDA ABS
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch next instr byte (MSB of abs addr), decode OP CODE, PC++
    //void fetch_adh_cycle3();
    //Cycle 4: fetch data from abs addr, poll for interrupts
    //void LDA_fetch_data();
    //Cycle 5 (start of next instr): fetch next OP CODE and finish LDA ABS (A <- data), flags set, PC++

    //LDA ABS, X
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch next instr byte (adh), add X to adl, PC++, If page boundary not crossed, curr_micro_op gets extra increment to skip over extra cycle
    //void read_ABS_X_cycle3()
    //Assuming Page boundary crossed
    //Cycle 4: Dummy read at mem(adh, adl+X), add 1 to adh
    //read_page_crossed_cycle()
    //Cycle 5: LDA_fetch_data(), poll for interrupts
    //Cycle 6 (start of next instr); fetch next opcode, A <-data, flags set, PC++
    //If page boundary NOT crossed
    //Cycle 4: LDA_fetch_data(), poll for interrupts
    //Cycle 5 (start of next instr): fetch next opcode, A <- data, flags set, PC++

    //LDA ABS,Y
    //Same but for cycle 3 we need: void ABS_Y_cycle3()

    //LDA (Indirect, X) i.e. preindexed
    //Cycle 1: fetch_opcode
    //Cycle 3: fetch_adl_cycle2()
    //Cycle 3: Dummy read, add X to indirect ZP address
    //ZP_X_cycle3()
    //Cycle 4: fetch adl using indirect ZP+X address
    //void IND_X_cycle4()
    //Cycle 5: fetch adh using indirect ZP+X+1 address
    //void IND_X_cycle5()
    //Cycle 6: fetch data using adh,adl
    //LDA_fetch_data()
    //Cycle 7 (strat of next instr): fetch next opcode, A <- data, flags set, PC++

    //LDA (Indirect),Y i.e. postindexed
    //Cycle 1: fetch_opcode
    //Cycle 2: fetch_adl_cycle2()
    //cycle 3: fetch the adl stored at the ZP address
    //void IND_Y_cycle3()
    //Cycle 4: fetch adh stored at ZP+1, add Y to adl
    //void read_IND_Y_cycle4()
    //Assume page boundary crossed
    //Cycle 5: dummy read mem(adh,adl+Y), add carry to adh
    //read_page_crossed_cycle()
    //Cycle 6: fetch data at mem(adh+1,adl+Y)
    //LDA_fetch_data()
    //Cycle 7 (start of next instr): fetch next opcode, A <- data, flags set, PC++
    //If page boundary wasn't crossed
    //Cycle 5: fetch data at mem(adh,adl+Y)
    //LDA_fetch_data()
    //Cycle 6 (start of next instr): fetch next opcode, A <- data, flags set, PC++

    //LDX, same as LDA but for X
    void LDX_IMM_cycle2();
    void load_X();
    void LDX_fetch_data();

    //LDY instructions, same as LDA but for Y
    void LDY_IMM_cycle2();
    void load_Y();
    void LDY_fetch_data();

    /** Data Movement (store ops) **/
    //STA ZP
    //Cycle 1: fetch_opcode
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: store A to mem at ZP, poll for interrupts
    void store_A();
    //No pipeline / overlap with next instr's fetch

    //STA ZP,X
    //Cycle 1: fetch_opcode
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: ZP_X_cycle3()
    //Cycle 4: store_A

    //STA ABS
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch_adh_cycle3()
    //Cycle 4: store_A()

    //STA ABS,X
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: write_ABS_X_cycle3()
    //Cycle 4: write_page_crossed_cycle()
    //Cycle 5: store_A()

    //STA ABS, Y
    //Same as above but cycle 3 uses write_ABS_Y_cycle3()

    //STA (Indirect, X) i.e. preindexed
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: ZP_X_cycle3()
    //Cycle 4: IND_X_cycle4()
    //Cycle 5: IND_X_cycle5()
    //Cycle 6: store_A()

    //STA (Indirect), Y i.e. postindexed
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: IND_Y_cycle3()
    //Cycle 4: write_IND_Y_cycle4()
    //Cycle 5: write_page_crossed_cycle()
    //Cycle 6: store_A()

    //STX instructions: similar to STA but for X
    void store_X();

    //STY instructions: similar to STA but for Y
    void store_Y();

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

    /** Arithmetic **/
    //INC ZP
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: RMW_read_cycle()
    //Cycle 4: dummy_write to ZP, ALU_result <- working_data + 1
    void INC_dummy_write();
    //Cycle 5: write ALU_result to ZP, update Z and N flags, poll for interrupts
    void INC_write_cycle();

    //The other addressing modes for INC ZP are obvious from other RMW instrs

    //DEC
    //Same as INC but uses decrement operation
    void DEC_dummy_write();
    void DEC_write_cycle();

    //INX
    //Cycle 1: fetch_opcode()
    //Cycle 2: dummy_read, decode opcode, hold PC
    void INX_cycle2();
    //Cycle 3 (start of next instr): fetch_opcode, ALU_result <- X+1
    void Increment();
    //Cycle 4: cycle 2 of next instr, X <- ALU_result, set Z,N flags
    void store_ALU2X_Increment();

    //INY: same as INX but for Y
    void INY_cycle2();
    void store_ALU2Y_Increment();

    //DEX and DEY: same as INX and INY but decrementing
    void DEX_cycle2();
    void DEY_cycle2();
    void Decrement();

    /** Shift instructions **/
    //ASL Accum
    //Cycle 1: fetch_opcode()
    //Cycle 2: dummy_read(), Decode opcode, hold PC
    void ASL_Accum_cycle2();
    //Cycle 3 (start of next instr): fetch_opcode, ALU_result <- A << 0
    void ASL();
    //Cycle 4: cycle 2 of next instr, A <- ALU_result, update flags Z,N,C
    void store_ALU2A_ASL();

    //ASL ZP
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch data at ZP
    //void RMW_read_cycle();
    //Cycle 4: dummy_write to ZP, ALU_result <- working_data << 1
    void ASL_dummy_write();
    //Cycle 5: write ALU_result to ZP, update Z,N,C flags, poll for interrupts
    void ASL_write_cycle();

    //ASL ZP, X
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: ZP_X_cycle3
    //Cycle 4: RMW_read_cycle()
    //Cycle 5: ASL_dummy_write
    //Cycle 6: ASL_write_cycle()

    //ASL ABS
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch_adh_cycle3()
    //Cycle 4: RMW_read_cycle()
    //Cycle 5: ASL_dummy_write()
    //Cycle 6: ASL_write_cycle()

    //ASL ABSX
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: write_ABS_X_cycle3()
    //Cycle 4: write_page_crossed_cycle()
    //Cycle 5: RMW_read_cycle()
    //Cycle 6: ASL_dummy_write()
    //Cycle 7: ASL_write_cycle()

    //LSR Accum
    //Cycle 1: fetch_opcode()
    //Cycle 2(): dummy_read(), Decode opcode, hold PC
    void LSR_Accum_cycle2();
    //Cycle 3 (start of next instr): fetch_opcode, ALU_result <- A << 0
    void LSR();
    //Cycle 4: cycle 2 of next instr, A <- ALU_result, update flags Z,N,C
    void store_ALU2A_LSR();

    //LSR ZP
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch data at ZP
    //void RMW_read_cycle();
    //Cycle 4: dummy_write to ZP, ALU_result <- working_data >> 1
    void LSR_dummy_write();
    //Cycle 5: write ALU_result to ZP, update Z,N,C flags, poll for interrupts
    void LSR_write_cycle();

    //LSR ZP,X, LSR ABS, and LSR ABS,X are obvious at this point

    //ROL
    void ROL_Accum_cycle2();
    void ROL();
    //void store_ALU2A_ASL();
    void ROL_dummy_write();
    //void ASL_write_cycle();

    //ROR
    void ROR_Accum_cycle2();
    void ROR();
    //void store_ALU2A_LSR();
    void ROR_dummy_write();
    //void LSR_write_cycle();

    /** Bitwise instructions **/
    //AND immediate
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch IMM val, Decode op, PC++, poll for interrupts
    void AND_IMM_cycle2();
    //Cycle 3 (start of next instr): fetch_opcode(), result = A & val, update flags
    void AND();
    //Cycle 4: whatever cycle 2 is of next instr, A <- result
    void store_ALU2A();

    //AND ZP
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch ZP address, Decode op, PC++
    //fetch_adl_cycle2()
    //Cycle 3: fetch data at ZP address, poll for interrupts
    void AND_final_cycle();
    //Cycle 4 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 5: whatever cycle 2 is of next instr, A <- result

    //AND ZP,X
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: ZP_X_cycle3()
    //Cycle 4: AND_final_cycle()
    //Cycle 5 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 6: whatever cycle 2 is of next instr, A <- result

    //AND ABS
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: fetch_adh_cycle3()
    //Cycle 4: AND_final_cycle()
    //Cycle 5 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 6: whatever cycle 2 is of next instr, A <- result

    //AND ABS,X
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: read_ABS_X_cycle3()
    //Cycle 4: read_page_crossed_cycle()
    //Cycle 5: AND_final_cycle()
    //Cycle 6 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 7: whatever cycle 2 is of next instr, A <- result

    //AND ABS,Y
    //Same as ABS,X but uses Y as index

    //AND indexed indirect i.e. preindexed
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: ZP_X_cycle3()
    //Cycle 4: IND_X_cycle4()
    //Cycle 5: IND_X_cycle5()
    //Cycle 6: AND_final_cycle()
    //Cycle 7 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 8: whatever cycle 2 is of next instr, A <- result

    //AND indirect indexed i.e postindexed
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3: IND_Y_cycle3()
    //Cycle 4: read_IND_Y_cycle4()
    //Assumming page cross
    //Cycle 5: read_page_crossed_cycle()
    //Cycle 6: AND_final_cycle()
    //Cycle 7 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 8: whatever cycle 2 is of next instr, A <- result
    //No page cross:
    //Cycle 5: read_page_crossed_cycle
    //Cycle 6 (start of next instr): fetch_opcode(), result = A & data, update flags
    //Cycle 7: whatever cycle 2 is of next instr, A <- result

    //OR instructions
    //Same as AND instructions but final cycle will use OR_final_cycle
    void OR_final_cycle();
    void OR();
    //Also, OR IMM will need
    void OR_IMM_cycle2();

    //EOR instructions
    //Same as AND instructions but final cycle will use EOR_final_cycle
    void EOR_final_cycle();
    void EOR();
    void EOR_IMM_cycle2();

    //OR and EOR Need special function b/c flags are set in same cycle as A unlike AND
    void store_ALU2A_set_Z_N();

    //BIT ZP
    //Cycle 1: fetch_opcode()
    //Cycle 2: fetch_adl_cycle2()
    //Cycle 3:
    void BIT_final_cycle();
    //Cycle 4 (start of next instr): fetch_opcode(), result = A & data, update Zero flag, bit 7 and 6 of data are used to update N and V
    void BIT();
    //Cycle 7: whatever cycle 2 is of next instr, result is discarded


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

    /** Jump Instructions **/
    //JMP Abs
    //Cycle 1: fetch OP CODE and finish prev op (overlap_op1), PC++
    //Cycle 2: fetch adl, decode OP CODE, PC++, check overlap_op2
    //void fetch_adl_cycle2();
    //Cycle 3: fetch adh, PC <-- adh, adl, poll for interrupts
    void JMP_ABS_cycle3();
    //Cycle 4 (start of next instr): fetch next OP CODE, no overlapped op, PC++

    /** Stack **/
    //Helper function to push value onto stack
    void push(uint16_t& address, uint8_t val);
    uint8_t pull(uint16_t& address);

    /** Interrupt Sequences **/
    /* Note: We do not poll for interrupts during interrupt sequences */
    //RESET
    //Cycles 1-5: don't really matter it seems
    //Cycle 6: Fetch address of RESET handler
    void fetch_RESET_vector_LSB();
    //Cycle 7: Fetch address of RESET handler
    void fetch_RESET_vector_MSB();

    //IRQ
    //Cycle 1: dummy read and finish prev op (overlap_op1)
    void interrupt_seq_cycle1();
    //Cycle 2: dummy read, check overlap_op2
    void interrupt_seq_cycle2();
    //Cycle 3: Push PCH, SP--
    void interrupt_seq_cycle3();
    //Cycle 4: Push PCL, SP--
    void interrupt_seq_cycle4();
    //Cycle 5: Push status reg, SP--
    void interrupt_seq_cycle5();
    //Cycle 6: Fetch address (LSB) of IRQ handler at $FFFE
    void IRQ_cycle6();
    //Cycle 7: Fetch address (MSB) of IRQ handler at $FFFF, set PC to IRQ handler, SET I <- 1
    void IRQ_cycle7();
    //Cycle 8 (1rst instr of handler): fetch_opcode

    //NMI
    //Has the same sequence as IRQ except that handler is at $FFFA - $FFFB
    void NMI_cycle6();
    void NMI_cycle7();

    //RTI
    //Cycle 1: fetch_opcode() (finish prev op - overlap_op1, PC++)
    //Cycle 2: dummy_read(), decode OP CODE, check overlap_op2
    void RTI_cycle2();
    //Cycle 3: dummy read the stack, sp++
    void RTI_cycle3();
    //Cycle 4: pull P from the stack, sp++
    void RTI_cycle4();
    //Cycle 5: pull PCL from the stack, sp++, restore P register with fetched val from cycle 4
    void RTI_cycle5();
    //Cycle 6: pull PCH from the stack, update PC, poll for interrupts
    void RTI_cycle6();
    //Cycle 7 (start of next instr): fetch_opcode()

};



#endif //CPU_H
