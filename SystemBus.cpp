
#include "SystemBus.hpp"

SystemBus::SystemBus(){
    //Clear contents of temp memory array
    for (int i = 0; i < RAM.size(); i++) RAM[i] = 0;

}

SystemBus::~SystemBus() {
}

void SystemBus::write(uint16_t address, uint8_t data) {
    if (address >= RAM_START && address <= MIRROR_RAM_END)
        RAM[address & RAM_END] = data; //handle RAM mirroring
    else if (address >= PRG_ROM_START && address <= PRG_ROM_END)
        ;//Can't write to PRG-ROM, ignore mem write request

}

uint8_t SystemBus::read(uint16_t address) {
    if (address >= RAM_START && address <= MIRROR_RAM_END)
        return RAM[address & RAM_END]; //handle RAM mirroring
    else if (address >= PRG_ROM_START && address <= PRG_ROM_END)
        return cartridge_ptr->readPRG(address);

    return 0x00; //NES ROM is using undefined behavior
}

void SystemBus::connect2cartridge(Cartridge* cartridge) {
    this->cartridge_ptr = cartridge;
}