#include <iostream>

#include "definitions.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "mmu.hpp"

int main() {
//    Cartrigde cart("../../gbemu/roms/Tetris (World) (Rev A).gb");
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/01-special.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/02-interrupts.gb"); // unimplemented?
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/03-op sp,hl.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/04-op r,imm.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/05-op rp.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/06-ld r,r.gb"); // pass
    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // fail hangs up
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/08-misc instrs.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/09-op r,r.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/10-bit ops.gb"); // pass
//    Cartrigde cart("../../gbemu/roms/cpu_instrs/individual/11-op a,(hl).gb"); // pass
    MMU mmu(cart);
    CPU cpu(mmu);
    bool done = false;
    size_t cycles;

    while (!done) {
        cycles = 0;
        while (cycles < CYCLES_PER_FRAME) {
            cycles += cpu.step();

            // blarggs test - serial output
            if (mmu.read(0xff02) == 0x81) {
                std::cout << mmu.read(0xff01);
                mmu.write(0xff02, 0);
            }
        }

        // TODO step(cycles) other modules
    }

    return 0;
}
