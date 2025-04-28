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
    //nestest
    //cpu.PC = 0xC000;
    //cpu.fetch_opcode();

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

        //TODO: debug
        //std::cout << "Cycle: " << cycles++ << std::endl;

        //Execute CPU cycle
        auto nextCycle = cpu.getNextFunctionPtr(cpu.opcode);
        if (nextCycle == nullptr)
            continue;
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


        //Terminate for nestest test rom
        //if (cpu.PC == 0xC6BD)
        //    break;

    }

    //Nestest - dump memory
    /*
    for (int i = 0; i<0x0800; i++)
        if (cpu.read(i) != 0)
                std::cout << "mem(" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << i << ") = " << (int)cpu.read(i) << std::endl;

    */
    //Nestest - print results
    //std::cout << "mem(" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << 0x0020 << ") = " << (int)cpu.read(0x0020) << std::endl;
    //std::cout << "mem(" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << 0x0030 << ") = " << (int)cpu.read(0x0030) << std::endl;
}
