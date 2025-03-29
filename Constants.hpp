#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdlib>

/**** All sizes are in # of bytes, unless otherwise specified ****/
constexpr size_t KIBIBYTE = 1024;

/** iNES file format constants **/
constexpr size_t HEADER_SIZE = 16;
constexpr size_t TRAINER_SIZE = 512; //the trainer size, if it exists

#endif //CONSTANTS_HPP
