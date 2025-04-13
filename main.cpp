#include <iostream>

#include "NES.hpp"
#include "global_vars.hpp"

bool stepMode = false;

int main(int argc, char* argv[]) {
    /** check cmd line args **/
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> <step_mode (true|false)>\n";
        return 1;
    }

    std::string rom_path = argv[1];
    std::string step_arg = argv[2];

    for (auto& c : step_arg) c = std::tolower(c);
    if (step_arg == "true") {
        stepMode = true;
    } else if (step_arg == "false") {
        stepMode = false;
    } else {
        std::cerr << "Invalid value for step_mode. Use true/false or 1/0.\n";
        return 1;
    }

    /** Startup system and read in ROM image **/
    NES nes;
    nes.initialize(rom_path);

    /** Run Emulator **/
    nes.run();

    return 0;
}

