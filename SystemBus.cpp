
#include "SystemBus.hpp"

SystemBus::SystemBus(){
    //Clear contents of temp memory array
    for (int i = 0; i < RAM.size(); i++) RAM[i] = 0;

}

SystemBus::~SystemBus() {
}

std::string SystemBus::getMemSpaceString(uint16_t address) {
    if (address >= RAM_START && address <= MIRROR_RAM_END)
        return "RAM";
    else if (address >= PRG_ROM_START && address <= PRG_ROM_END)
        return "PRG-ROM";
    else
        return "unimplemented";
}

void SystemBus::write(uint16_t address, uint8_t data) {
    if (address >= RAM_START && address <= MIRROR_RAM_END)
        RAM[address & RAM_END] = data; //handle RAM mirroring
    else if (address >= PRG_ROM_START && address <= PRG_ROM_END)
        ;//Can't write to PRG-ROM, ignore mem write request

    //Handling logging / stepMode
    if (stepMode) {
        std::cout << "mem($" << std::hex
                << std::uppercase
                << std::setw(4)
                << std::setfill('0')
                << (int)address
                << ", W, "
                << getMemSpaceString(address)
                << "): $"
                << (int)data
                << "\n";
    }
}

uint8_t SystemBus::read(uint16_t address) {
    uint8_t data = 0x00;
    if (address >= RAM_START && address <= MIRROR_RAM_END)
        data = RAM[address & RAM_END]; //handle RAM mirroring
    else if (address >= PRG_ROM_START && address <= PRG_ROM_END)
        data = cartridge_ptr->readPRG(address);

    //Handle logging / stepMode
    if (stepMode) {
        std::cout << "mem($" << std::hex
                << std::uppercase
                << std::setw(4)
                << std::setfill('0')
                << (int)address
                << ", R, "
                << getMemSpaceString(address)
                << "): $"
                << (int)data
                << "\n";
    }

    return data;
}

void SystemBus::connect2cartridge(Cartridge* cartridge) {
    this->cartridge_ptr = cartridge;
}