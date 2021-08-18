#include <iostream>

#include "definitions.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "mmu.hpp"

int main() {
//    Cartrigde cart("../../gbemu/roms/Tetris (World) (Rev A).gb");
    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/09-op r,r.gb");
    CPU cpu{};
    MMU mmu;
    bool done = false;
    size_t cycles;

    while (!done) {
        cycles = 0;
        while (cycles < CYCLES_PER_FRAME) {
            cycles += cpu.step();

            // blarggs test - serial output
            if (mmu.read((u8) 0xff02) == 0x81) {
                std::cout << mmu.read((u8) 0xff01);
                mmu.write((u8) 0xff02, 0);
            }
        }

        // TODO step(cycles) other modules
    }

    return 0;
}
