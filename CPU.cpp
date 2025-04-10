
#include "CPU.hpp"

uint64_t cycles = 0;

CPU::CPU() {
    //Initialize the opcode matrix
    opMatrix.resize(16*16);
    //CLC
    opMatrix[0x18].pneumonic = "CLC";
    opMatrix[0x18].addressing_mode = IMP;
    opMatrix[0x18].cycle_op_list.push_back(&CPU::CLC_cycle2);
    opMatrix[0x18].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEC
    opMatrix[0x38].pneumonic = "SEC";
    opMatrix[0x38].addressing_mode = IMP;
    opMatrix[0x38].cycle_op_list.push_back(&CPU::SEC_cycle2);
    opMatrix[0x38].cycle_op_list.push_back(&CPU::fetch_opcode);
    //JMP ABS
    opMatrix[0x4C].pneumonic = "JMP";
    opMatrix[0x4C].addressing_mode = ABS;
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::ABS_cycle2);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::JMP_ABS_cycle3);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLI
    opMatrix[0x58].pneumonic = "CLI";
    opMatrix[0x58].addressing_mode = IMP;
    opMatrix[0x58].cycle_op_list.push_back(&CPU::CLI_cycle2);
    opMatrix[0x58].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEI
    opMatrix[0x78].pneumonic = "SEI";
    opMatrix[0x78].addressing_mode = IMP;
    opMatrix[0x78].cycle_op_list.push_back(&CPU::SEI_cycle2);
    opMatrix[0x78].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXA
    opMatrix[0x8A].pneumonic = "TXA";
    opMatrix[0x8A].addressing_mode = IMP;
    opMatrix[0x8a].cycle_op_list.push_back(&CPU::TXA_cycle2);
    opMatrix[0x8a].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TYA
    opMatrix[0x98].pneumonic = "TYA";
    opMatrix[0x98].addressing_mode = IMP;
    opMatrix[0x98].cycle_op_list.push_back(&CPU::TYA_cycle2);
    opMatrix[0x98].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXS
    opMatrix[0x9A].pneumonic = "TXS";
    opMatrix[0x9A].addressing_mode = IMP;
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::TXS_cycle2);
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA0].pneumonic = "LDY";
    opMatrix[0xA0].addressing_mode = IMM;
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::LDY_IMM_cycle2);
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA2].pneumonic = "LDX";
    opMatrix[0xA2].addressing_mode = IMM;
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::LDX_IMM_cycle2);
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAY
    opMatrix[0xA8].pneumonic = "TAY";
    opMatrix[0xA8].addressing_mode = IMP;
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::TAY_cycle2);
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA IMM
    opMatrix[0xA9].pneumonic = "LDA";
    opMatrix[0xA9].addressing_mode = IMM;
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::LDA_IMM_cycle2);
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAX
    opMatrix[0xAA].pneumonic = "TAX";
    opMatrix[0xAA].addressing_mode = IMP;
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::TAX_cycle2);
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TSX
    opMatrix[0xBA].pneumonic = "TSX";
    opMatrix[0xBA].addressing_mode = IMP;
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::TSX_cycle2);
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLV
    opMatrix[0xB8].pneumonic = "CLV";
    opMatrix[0xB8].addressing_mode = IMP;
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::CLV_cycle2);
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLD
    opMatrix[0xD8].pneumonic = "CLD";
    opMatrix[0xD8].addressing_mode = IMP;
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::CLD_cycle2);
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SED
    opMatrix[0xFD].pneumonic = "SED";
    opMatrix[0xFD].addressing_mode = IMP;
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::SED_cycle2);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::fetch_opcode);
}

CPU::~CPU() {
    //TODO
}

void CPU::setStatusReg(bool set, Flags flag) {
    if (set)
        status_reg |= (1 << flag);
    else
        status_reg &= ~(1 << flag);
}

bool CPU::getStatusReg(Flags flag)  {
    return (status_reg & (1 << flag));
}

void CPU::connect2Bus(SystemBus *bus_ptr) {
    system_bus_ptr = bus_ptr;
}

uint8_t CPU::read(uint16_t address) {
    return system_bus_ptr->read(address);
}

void CPU::write(uint16_t address, uint8_t data) {
    system_bus_ptr->write(address, data);
}

std::string CPU::getPneumonic(uint8_t opcode) {
    return opMatrix[opcode].pneumonic;
}

AddressingMode CPU::getAddressingMode(uint8_t opcode) {
    return opMatrix[opcode].addressing_mode;
}

CPU::cycle_operation CPU::getNextFunctionPtr(uint8_t opcode) {
    return opMatrix[opcode].cycle_op_list[curr_micro_op];
}

size_t CPU::getListSize(uint8_t opcode) {
    return opMatrix[opcode].cycle_op_list.size();
}

/****/

void CPU::fetch_opcode() {
    opcode = read(PC);
    PC++;

    if (overlap_op1 != nullptr)
        (this->*overlap_op1)();

    interrupt_poll_cycle = false;
    curr_micro_op = 0;
}

uint8_t CPU::fetch_operand_1byte() {
    //operand_1byte = read(PC);
    PC++;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::fetch_operand_2byte_LSB() {
    //operand_2byte = read(PC);
    PC++;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::fetch_operand_2byte_MSB() {
    //operand_2byte += (static_cast<uint16_t>(read(PC)) << static_cast<uint16_t>(8));
    PC++;
    curr_micro_op++;
    return 1;
}

void CPU::dummy_read() {
    read(PC);
}
/****/

/** Addressing Modes **/
void CPU::ABS_cycle2() {
    operand_byte1 = read(PC++);
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

/** Addressing Modes **/

uint8_t CPU::set_working_data_immediate() {
    //working_data = operand_1byte;
    curr_micro_op++;
    return 0;
}

uint8_t CPU::set_working_data_zero_page() {
    //working_data = read(operand_1byte);
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_zero_page_X() {
    //uint8_t zp_addr = operand_1byte + X;
    //working_data = read(zp_addr);
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_absolute() {
    //working_data = read(operand_2byte);
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_absolute_X_1() {
    //uint16_t abs_addr = operand_2byte + X;
    //if((operand_2byte & 0xFF00) != (abs_addr & 0xFF00)) {
        //Page crossed! Extra cycle needed
        //operand_2byte = abs_addr;
        dummy_read();
        return 1;
    //} else { //Page not crossed, skip the next micro_op
        //working_data = read(abs_addr);
        //instr_remaining_cycles -= 2; //We have to get rid of an extra cycle b/c we assumed instruction would take 5 cycles, but only takes 4
        curr_micro_op += 2; //Skip over next micro op
        return 1;
    //}

}

uint8_t CPU::set_working_data_absolute_X_2() {
    //working_data = read(operand_2byte); //Note: operand_2byte will have had X already added to it in set_working_data_absolute_X_1()
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_absolute_Y_1() {
    //uint16_t abs_addr = operand_2byte + Y;
    //if((operand_2byte & 0xFF00) != (abs_addr & 0xFF00)) {
        //Page crossed! Extra cycle needed
        //operand_2byte = abs_addr;
        dummy_read();
        return 1;
    //} else { //Page not crossed, skip the next micro_op
        //working_data = read(abs_addr);
        //instr_remaining_cycles -= 2; //We have to get rid of an extra cycle b/c we assumed instruction would take 5 cycles, but only takes 4
        curr_micro_op += 2; //Skip over next micro op
        return 1;
    //}
}

uint8_t CPU::set_working_data_absolute_Y_2() {
    //working_data = read(operand_2byte); //Note: operand_2byte will have had Y already added to it in set_working_data_absolute_Y_1()
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indexed_indirect_1() {
    //operand_1byte += X; //uint8_t wraps mod 256
    //operand_2byte = read(operand_1byte); //LSB
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indexed_indirect_2() {
    //uint8_t msb_addr = operand_1byte + 1; //ensure 8 bit wrapping mod 256
    //operand_2byte += (read(msb_addr) << 8);
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indexed_indirect_3() {
    //working_data = read(operand_2byte);
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indirect_indexed_1() {
    //operand_2byte = read(operand_1byte);
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indirect_indexed_2() {
    //uint8_t msb_addr = operand_1byte + 1; //ensure 8 bit wrapping mod 256
    //operand_2byte += (read(msb_addr) << 8);
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_indirect_indexed_3() {
    //uint16_t final_addr = operand_2byte + Y;
    //if((operand_2byte & 0xFF00) != (final_addr & 0xFF00)) {
        //Page crossed! Extra cycle needed
        //operand_2byte = final_addr;
        dummy_read();
        return 1;
    //} else { //Page not crossed, skip the next micro_op
        //working_data = read(final_addr);
        //instr_remaining_cycles -= 2; //We have to get rid of an extra cycle b/c we skip an operation that takes 1 cycle that we assummed would happen
        curr_micro_op += 2; //Skip over next micro op
        return 1;
    //}
}

uint8_t CPU::set_working_data_indirect_indexed_4() {
    //working_data = read(operand_2byte); //Note: operand_2byte will have had Y already added to it in set_working_data_indirect_indexed_3()
    //instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::copy_operand1to2() {
    //operand_2byte = operand_1byte;
    return 0;
}

/** Data Movement (access) **/
void CPU::LDA_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_A() {
    A = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::LDX_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_X() {
    X = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::LDY_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::load_Y;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_Y() {
    Y = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

/** Data Movement (transfer) **/
void CPU::TAX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = A;

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TXA_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = X;

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TAY_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = A;

    overlap_op1 = &CPU::load_Y;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TYA_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = Y;

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TSX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = SP;

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TXS_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = X;

    overlap_op1 = &CPU::load_SP;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_SP() {
    SP = working_data;
    //Don't set Z and N flags
}

/** FLAG instructions **/
void CPU::CLC_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_carry;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::clear_carry() {
    setStatusReg(false, C);
}

void CPU::CLI_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_interrupt_disable;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::clear_interrupt_disable() {
    setStatusReg(false, I);
}

void CPU::CLV_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_overflow;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::clear_overflow() {
    setStatusReg(false, V);
}

void CPU::CLD_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_decimal;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::clear_decimal() {
    setStatusReg(false, D);
}

void CPU::SEC_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_carry;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::set_carry() {
    setStatusReg(true, C);
}

void CPU::SEI_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_interrupt_disable;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::set_interrupt_disable() {
    setStatusReg(true, I);
}

void CPU::SED_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_decimal;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::set_decimal() {
    setStatusReg(true, D);
}

/****/
uint8_t CPU::AND() {
    A &= working_data;

    if (A == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((A & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    curr_micro_op++;
    return 0;
}

/** Jump Instructions **/
void CPU::JMP_ABS_cycle3() {
    operand_byte2 = read(PC++);

    PC = (static_cast<uint16_t>(operand_byte2) << 8) | operand_byte1;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}





