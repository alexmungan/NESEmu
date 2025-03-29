#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdlib>

/**** All sizes are in # of bytes, unless otherwise specified ****/

extern constexpr size_t KIBIBYTE;

/** iNES file format constants **/
extern constexpr size_t HEADER_SIZE;
extern constexpr size_t TRAINER_SIZE; //the trainer size, if it exists

#endif //CONSTANTS_HPP
