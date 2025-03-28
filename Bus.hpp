//Bus Class: implements the virtual system bus (data bus)
    //Serves as connection between system components
//It's not necessary to explicitly implement address bus

#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>

#include "CPU.hpp"

class Bus {
public:
    //Constructor
    Bus();
    ~Bus();

    //Devices connected to the bus
    CPU* cpu; //Ricoh 2A03
    //TODO: Other devices the bus is connected to?
    //PPU (for CPU-PPU communication via writes to mem mapped PPU registers)
    //APU (for CPU-APU communication via writes to mem mapped APU registers)
    //Cartridge (to read from CHR/PRG-ROM and write to cartridge RAM if available)
    //I/O?????
    std::array<uint8_t, 64 * 1024> RAM; //Memory space (64 KB) for now (later will be the 2KB of system RAM

    //CPU puts a value on the data bus and address on implied address bus
    //The address corresponds to the address range of some other device
    //connected to the data bus. This device will have the contents at
    //address written to -- may be a mm register, system RAM, ...
    void write(uint16_t address, uint8_t data);

    //CPU outputs address
    //The address correspond to the address range of some other device
    //connected to the data bus. This device will go to this address and
    //place its contents on the data bus for the CPU to read.
    uint8_t read(uint16_t address, bool bReadOnly = false);
};



#endif //BUS_H
