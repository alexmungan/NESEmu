
#include "CPU.hpp"

uint64_t cycles = 0;

CPU::CPU() {
    //Initialize the opcode matrix https://web.archive.org/web/20221112231348if_/http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    opMatrix.resize(16*16);
    //CLC
    opMatrix[0x18].pneumonic = "CLC";
    opMatrix[0x18].addressing_mode = IMP;
    opMatrix[0x18].cycle_op_list.push_back(waste_cycle);
    opMatrix[0x18].cycle_op_list.push_back(clear_carry);
    opMatrix[0x18].cycles = 2;

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
    return opMatrix[opcode].cycle_op_list[(instr_remaining_cycles - 1)];
}

uint8_t CPU::getCycles(uint8_t opcode) {
    return opMatrix[opcode].cycles;
}

/****/

uint8_t CPU::fetch_opcode() {
    opcode = read(PC);
    PC++;
    instr_remaining_cycles = getCycles(opcode);
    instr_remaining_cycles--;
    return 1;
}

uint8_t CPU::waste_cycle() {
    instr_remaining_cycles--;
    return 1;
}

uint8_t CPU::clear_carry() {
    setStatusReg(false, C);
    return 0;
}





