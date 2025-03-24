
#include "Bus.hpp"

Bus::Bus() {
    //Clear contents of temp memory array
    for (int i = 0; i < mem.size(); i++) mem[i] = 0;
}

Bus::~Bus() {

}

void Bus::write(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0xFFFF)
        mem[address] = data;
}

uint8_t Bus::read(uint16_t address, bool bReadOnly = false) {
    if (address >= 0x0000 && address <= 0xFFFF)
        return mem[address];

    return 0x00; //TODO
}