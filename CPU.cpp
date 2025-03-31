
#include "CPU.hpp"

uint64_t cycles = 0;

CPU::CPU() {
    //Initialize the opcode matrix https://web.archive.org/web/20221112231348if_/http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    opMatrix.resize(16*16);
    //CLC
    opMatrix[0x18].pneumonic = "CLC";
    opMatrix[0x18].addressing_mode = IMP;
    opMatrix[0x18].cycle_op_list.push_back(&CPU::clear_carry);
    opMatrix[0x18].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0x18].cycles = 2;
    //AND Zero Page
    opMatrix[0x25].pneumonic = "AND";
    opMatrix[0x25].addressing_mode = ZP;
    opMatrix[0x25].cycle_op_list.push_back(&CPU::fetch_operand_1byte);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::set_working_data_zero_page);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::AND);
    opMatrix[0x25].cycles = 3;
    //AND IMM
    opMatrix[0x29].pneumonic = "AND";
    opMatrix[0x29].addressing_mode = IMM;
    opMatrix[0x29].cycle_op_list.push_back(&CPU::fetch_operand_1byte);
    opMatrix[0x29].cycle_op_list.push_back(&CPU::set_working_data_immediate);
    opMatrix[0x29].cycle_op_list.push_back(&CPU::AND);
    opMatrix[0x29].cycles = 2;
    //AND Absolute
    opMatrix[0x2D].pneumonic = "AND";
    opMatrix[0x2D].addressing_mode = ABS;
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_operand_2byte_LSB);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_operand_2byte_MSB);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::set_working_data_absolute);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::AND);
    opMatrix[0x2D].cycles = 4;
    //AND ZP X
    opMatrix[0x35].pneumonic = "AND";
    opMatrix[0x35].addressing_mode = ZPX;
    opMatrix[0x35].cycle_op_list.push_back(&CPU::fetch_operand_1byte);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::set_working_data_zero_page_X);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::AND);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0x35].cycles = 4;
    //SEC
    opMatrix[0x38].pneumonic = "SEC";
    opMatrix[0x38].addressing_mode = IMP;
    opMatrix[0x38].cycle_op_list.push_back(&CPU::set_carry);
    opMatrix[0x38].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0x38].cycles = 2;
    //AND Absolute X
    opMatrix[0x3D].pneumonic = "AND";
    opMatrix[0x3D].addressing_mode = ABSX;
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_operand_2byte_LSB);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_operand_2byte_MSB);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::set_working_data_absolute_X_1);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::set_working_data_absolute_X_2);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::AND);
    opMatrix[0x3D].cycles = 5; //Normally takes 4 cycles, 5 if boundary is crossed
    //CLI
    opMatrix[0x58].pneumonic = "CLI";
    opMatrix[0x58].addressing_mode = IMP;
    opMatrix[0x58].cycle_op_list.push_back(&CPU::clear_interrupt_disable);
    opMatrix[0x58].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0x58].cycles = 2;
    //SEI
    opMatrix[0x78].pneumonic = "SEI";
    opMatrix[0x78].addressing_mode = IMP;
    opMatrix[0x78].cycle_op_list.push_back(&CPU::set_interrupt_disable);
    opMatrix[0x78].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0x78].cycles = 2;
    //CLV
    opMatrix[0xB8].pneumonic = "CLV";
    opMatrix[0xB8].addressing_mode = IMP;
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::clear_overflow);
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0xB8].cycles = 2;
    //CLD
    opMatrix[0xD8].pneumonic = "CLD";
    opMatrix[0xD8].addressing_mode = IMP;
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::clear_decimal);
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0xD8].cycles = 2;
    //SED
    opMatrix[0xF8].pneumonic = "SED";
    opMatrix[0xF8].addressing_mode = IMP;
    opMatrix[0xF8].cycle_op_list.push_back(&CPU::set_decimal);
    opMatrix[0xF8].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[0xF8].cycles = 2;

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

uint8_t CPU::getCycles(uint8_t opcode) {
    return opMatrix[opcode].cycles;
}

size_t CPU::getListSize(uint8_t opcode) {
    return opMatrix[opcode].cycle_op_list.size();
}

/****/

uint8_t CPU::fetch_opcode() {
    opcode = read(PC);
    PC++;
    operand_1byte = 0x00;
    operand_2byte = 0x0000;
    working_data = 0x00;
    instr_remaining_cycles = getCycles(opcode);
    curr_micro_op = 0;
    cycle_op_list_size = getListSize(opcode);
    instr_remaining_cycles--;
    return 1;
}

uint8_t CPU::fetch_operand_1byte() {
    operand_1byte = read(PC);
    PC++;
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::fetch_operand_2byte_LSB() {
    operand_2byte = read(PC);
    PC++;
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::fetch_operand_2byte_MSB() {
    operand_2byte += (static_cast<uint16_t>(read(PC)) << static_cast<uint16_t>(8));
    PC++;
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}
/****/

uint8_t CPU::set_working_data_immediate() {
    working_data = operand_1byte;
    curr_micro_op++;
    return 0;
}

uint8_t CPU::set_working_data_zero_page() {
    working_data = read(operand_1byte);
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_zero_page_X() {
    uint8_t zp_addr = operand_1byte + X;
    working_data = read(zp_addr);
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_absolute() {
    working_data = read(operand_2byte);
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::set_working_data_absolute_X_1() {
    uint16_t abs_addr = operand_2byte + X;
    if((operand_2byte & 0xFF00) != (abs_addr & 0xFF00)) {
        //Page crossed! Extra cycle needed
        operand_2byte = abs_addr;
        return waste_cycle();
    } else { //Page not crossed, skip the next micro_op
        working_data = read(abs_addr);
        instr_remaining_cycles -= 2; //We have to get rid of an extra cycle b/c we assumed instruction would take 5 cycles, but only takes 4
        curr_micro_op += 2; //Skip over next micro op
        return 1;
    }

}

uint8_t CPU::set_working_data_absolute_X_2() {
    working_data = read(operand_2byte); //Note: operand_2byte will have had X already added to it in set_working_data_absolute_X_1()
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}


/****/

uint8_t CPU::waste_cycle() {
    instr_remaining_cycles--;
    curr_micro_op++;
    return 1;
}

uint8_t CPU::clear_carry() {
    setStatusReg(false, C);
    curr_micro_op++;
    return 0;
}

uint8_t CPU::clear_interrupt_disable() {
    setStatusReg(false, I);
    curr_micro_op++;
    return 0;
}

uint8_t CPU::clear_overflow() {
    setStatusReg(false, V);
    curr_micro_op++;
    return 0;
}

uint8_t CPU::clear_decimal() {
    setStatusReg(false, D);
    curr_micro_op++;
    return 0;
}

uint8_t CPU::set_carry() {
    setStatusReg(true, C);
    curr_micro_op++;
    return 0;
}

uint8_t CPU::set_interrupt_disable() {
    setStatusReg(true, I);
    curr_micro_op++;
    return 0;
}
uint8_t CPU::set_decimal() {
    setStatusReg(true, D);
    curr_micro_op++;
    return 0;
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





