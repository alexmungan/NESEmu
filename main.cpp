#include <iostream>
#include <chrono>
#include <thread>

#include "NES.hpp"

constexpr int  CPU_CYCLES_PER_FRAME = 29780; //1789733hz / 60.0988 fps = 29780.51142

//void runFrame();

int main() {

    //Read in .nes ROM image into memory

    //


    /*
    //Execution loop
    const int FPS = 60;
    const std::chrono::milliseconds FRAME_DURATION(1000 / FPS);
    auto last_timer_update = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds time_accumulator(0);

    while (running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_timer_update);
        time_accumulator += elapsed_time;
        last_timer_update = current_time;

        if (time_accumulator >= FRAME_DURATION) {
            runFrame();
        }
        else {
            std::this_thread::sleep_for(FRAME_DURATION - time_accumulator);
        }

    }
    */

    return 0;
}

/*
void runFrame() {
    handleInput(); // Read controller input

    int cyclesThisFrame = 0;
    while (cyclesThisFrame < CPU_CYCLES_PER_FRAME) { //TODO: Is there a way to know when PPU is done and stop then by signaling NMI?
        int cycles = cpu.executeNextInstruction(); // Execute one CPU instruction

        for (int i = 0; i < cycles * 3; i++) {
            ppu.step();  // Run PPU three times per CPU cycle
            if (ppu.generateNMI) {
                cpu.triggerNMI(); // Handle Vertical Blank (VBlank) NMI
            }
        }

        apu.step(cycles); // Step the audio processor

        cyclesThisFrame += cycles;
    }

    renderFrame(); // Display frame to screen
}
*/