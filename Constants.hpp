#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdlib>

/**** All sizes are in # of bytes, unless otherwise specified ****/
constexpr size_t KIBIBYTE = 1024;

/** iNES file format constants **/
constexpr size_t HEADER_SIZE = 16;
constexpr size_t TRAINER_SIZE = 512; //the trainer size, if it exists

/** Memory Layout constants (starting and ending addresses) **/
//System RAM
constexpr size_t RAM_START = 0x0000;
constexpr size_t RAM_END = 0x07FF;
constexpr size_t MIRROR_RAM_END = 0x1FFF; //End of mirrored RAM
//... TODO
//PRG-ROM
constexpr size_t PRG_ROM_START = 0x8000;
constexpr size_t PRG_ROM_END = 0xFFFF;
constexpr size_t BANK1_END = 0xBFFF;

//Stack
constexpr size_t STACK_START = 0x0100;

/** CPU constants **/
//Opcodes for my pseudo opcodes for interrupts
constexpr uint16_t RESET_opcode = 0x0100;
constexpr uint16_t NMI_opcode = 0x0101;
constexpr uint16_t IRQ_opcode = 0x0102;

#endif //CONSTANTS_HPP
