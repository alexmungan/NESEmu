#include "NES.hpp"

NES::NES() {
}

NES::~NES() {

}

void NES::initialize(const std::string &rom_path) {
    cartridge.loadROM(rom_path);
    systemBus.connect2cartridge(&cartridge);
    cpu.connect2Bus(&systemBus);
}

void NES::run() {
    //Output power on state TODO

    //RESET sequence TODO

    //Output initial state (after RESET) TODO

}
