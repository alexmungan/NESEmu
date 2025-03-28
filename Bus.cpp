
#include "Bus.hpp"

Bus::Bus() : cpu(this){
    //Clear contents of temp memory array
    for (int i = 0; i < RAM.size(); i++) RAM[i] = 0;

}

Bus::~Bus() {
    //TODO
}

void Bus::write(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0xFFFF)
        RAM[address] = data;
}

uint8_t Bus::read(uint16_t address, bool bReadOnly) {
    if (address >= 0x0000 && address <= 0xFFFF)
        return RAM[address];

    return 0x00; //TODO
}