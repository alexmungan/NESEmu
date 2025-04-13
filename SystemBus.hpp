//Bus Class: implements the virtual system bus (data bus)
//Serves as connection between system components
//Interrupt lines are implemented here aswell
//It's not necessary to explicitly implement address bus

#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "Cartridge.hpp"
#include "Constants.hpp"
#include "global_vars.hpp"

class SystemBus {
public:
    //Constructor
    SystemBus();
    ~SystemBus();

private:
    //Devices connected to the bus
    //Note: CPU makes requests to the bus and is of course connected to the bus, but
    //The connection is only needed from the CPU class
    //TODO: Other devices the bus is connected to?
    //PPU (for CPU-PPU communication via writes to mem mapped PPU registers)
    //APU (for CPU-APU communication via writes to mem mapped APU registers)
    Cartridge* cartridge_ptr = nullptr;
    //I/O?????
    std::array<uint8_t, 2 * KIBIBYTE> RAM; //2KB of system RAM

    //Helper function for logging. Gets a string representation of a memory space name that an address lives in
    std::string getMemSpaceString(uint16_t address);

public:
    /** Bus reading and writing **/
    /** Should not be done unless required devices have been connected **/
    //CPU puts a value on the data bus and address on implied address bus
    //The address corresponds to the address range of some other device
    //connected to the data bus. This device will have the contents at
    //address written to -- may be a mm register, system RAM, ...
    void write(uint16_t address, uint8_t data);

    //CPU outputs address
    //The address correspond to the address range of some other device
    //connected to the data bus. This device will go to this address and
    //place its contents on the data bus for the CPU to read.
    uint8_t read(uint16_t address);

    //Connect to devices
    void connect2cartridge(Cartridge* cartridge);
};



#endif //BUS_H
