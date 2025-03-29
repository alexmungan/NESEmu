
#include "SystemBus.hpp"

SystemBus::SystemBus(){
    //Clear contents of temp memory array
    for (int i = 0; i < RAM.size(); i++) RAM[i] = 0;

}

SystemBus::~SystemBus() {
}

void SystemBus::write(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0xFFFF)
        RAM[address] = data;
}

uint8_t SystemBus::read(uint16_t address) {
    if (address >= 0x0000 && address <= 0xFFFF)
        return RAM[address];

    return 0x00; //TODO
}

void SystemBus::connect2cartridge(Cartridge* cartridge) {
    this->cartridge_ptr = cartridge;
}