#include "NES.hpp"

//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/Audio.hpp>
//#include <SFML/System.hpp>

uint64_t cycles = 0;

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
    //Output power on state, RESET is signal is recognized during very 1rst cycle
    if (stepMode) {
        std::cout << "Cycle: " << cycles++ << std::endl;
        cpu.cpu_dump();
    }

    //After 1rst cycle, system will start executing the RESET interrupt sequence
    cpu.opcode = RESET_opcode;

    //TODO: temp for testing
    cpu.write(0x0002, 0x04); //adl
    cpu.write(0x0003, 0x06); //adh
    cpu.write(0x0604, 0x83); //val

    //Main execution loop (infinite loop)
    while (true) {
        //Temporary code until external devices such as PPU are implemented
        cpu.IRQ_signal = false;
        cpu.NMI_signal = false;
        cpu.RESET_signal = false;

        if (stepMode) {
            char c;
            while (true) {
                std::cin >> c;
                if (c == 'i') cpu.IRQ_signal = true;
                if (c == 'n') cpu.NMI_signal = true;
                if (c == 'r') cpu.RESET_signal = true;
                if (c == 's') break;
            }
            std::cout << "Cycle: " << cycles++ << std::endl;
        }

        //Execute CPU cycle
        auto nextCycle = cpu.getNextFunctionPtr(cpu.opcode);
        (cpu.*nextCycle)();

        //TODO: Execute 3 PPU cycles

        if (stepMode) {
            cpu.cpu_dump();
        }

        //Interrupt polling
        if (cpu.RESET_signal) {
            cpu.opcode = RESET_opcode;
            cpu.curr_micro_op = 0;
        }
        else if (cpu.interrupt_poll_cycle && cpu.NMI) {
            std::cout << "HERE\n";
            cpu.opcode = NMI_opcode;
            cpu.curr_micro_op = 0;
        }
        else if (cpu.interrupt_poll_cycle && cpu.IRQ) {
            cpu.opcode = IRQ_opcode;
            cpu.curr_micro_op = 0;
        }

        //Interrupt signals - edge/level detectors
        if (cpu.IRQ_signal)
            cpu.IRQ = true;
        else
            cpu.IRQ = false;
        if (cpu.NMI_signal)
            cpu.NMI = true;
        else
            cpu.NMI = false;


    }

}
