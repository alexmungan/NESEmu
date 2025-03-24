
#include "CPU.hpp"

CPU::CPU(Bus* bus_ptr) {
    this->bus = bus_ptr;

    //Initialize the instruction matrix https://web.archive.org/web/20221112231348if_/http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    instrMatrix  = {
        //todo
    }
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

bool CPU::getStatusReg(Flags flag) {
    return (status_reg & (1 << flag));
}

uint8_t CPU::getInstrData() {
    //TODO
}


uint8_t CPU::read(uint16_t address) {
    return bus->read(address);
}

void CPU::write(uint16_t address, uint8_t data) {
    bus->write(address, data);
}

