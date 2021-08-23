#pragma once
#include <memory>
#include <utility>
#include "mmu.hpp"

struct Registers {
    struct {
        union {
            struct {
                u8 f;
                u8 a;
            };
            u16 af;
        };
    };

    struct {
        union {
            struct {
                u8 c;
                u8 b;
            };
            u16 bc;
        };
    };

    struct {
        union {
            struct {
                u8 e;
                u8 d;
            };
            u16 de;
        };
    };

    struct {
        union {
            struct {
                u8 l;
                u8 h;
            };
            u16 hl;
        };
    };

    u16 sp;
    u16 pc;
};

class CPU {
public:
    explicit CPU(MMU& mmu) : mmu(mmu) {}

    size_t step() {
        // TODO handle interrupts

        if (halted) { return 4; } else { return exec(); }
    }

private:
    Registers registers{0xb0, 0x01, 0x13, 0, 0xd8, 0, 0x4D, 0x01, 0xfffe, 0x100};
    MMU& mmu;
    bool halted = false;

    size_t exec() {
        u8 op = read_u8();

        switch (op) {
            case 0x00: break;
            case 0x01: ld_rp_nn<0>(); break;
            case 0x02: ldid_a_nn<0>(); break;
            case 0x06: ld_r_n<0>(); break;
            case 0x0a: ldid_nn_a<0>(); break;
            case 0x0e: ld_r_n<1>(); break;
            case 0x11: ld_rp_nn<1>(); break;
            case 0x12: ldid_a_nn<1>(); break;
            case 0x16: ld_r_n<2>(); break;
            case 0x1a: ldid_nn_a<1>(); break;
            case 0x1e: ld_r_n<3>(); break;
            case 0x21: ld_rp_nn<2>(); break;
            case 0x22: ldid_a_nn<2>(); break;
            case 0x26: ld_r_n<4>(); break;
            case 0x2a: ldid_nn_a<2>(); break;
            case 0x2e: ld_r_n<5>(); break;
            case 0x31: ld_rp_nn<3>(); break;
            case 0x32: ldid_a_nn<3>(); break;
            case 0x36: ld_r_n<6>(); break;
            case 0x3a: ldid_nn_a<3>(); break;
            case 0x3e: ld_r_n<7>(); break;
            case 0x40: ld_r1_r2<0, 0>(); break; case 0x41: ld_r1_r2<0, 1>(); break; case 0x42: ld_r1_r2<0, 2>(); break;
            case 0x43: ld_r1_r2<0, 3>(); break; case 0x44: ld_r1_r2<0, 4>(); break; case 0x45: ld_r1_r2<0, 5>(); break;
            case 0x46: ld_r1_r2<0, 6>(); break; case 0x47: ld_r1_r2<0, 7>(); break; case 0x48: ld_r1_r2<1, 0>(); break;
            case 0x49: ld_r1_r2<1, 1>(); break; case 0x4a: ld_r1_r2<1, 2>(); break; case 0x4b: ld_r1_r2<1, 3>(); break;
            case 0x4c: ld_r1_r2<1, 4>(); break; case 0x4d: ld_r1_r2<1, 5>(); break; case 0x4e: ld_r1_r2<1, 6>(); break;
            case 0x4f: ld_r1_r2<1, 7>(); break; case 0x50: ld_r1_r2<2, 0>(); break; case 0x51: ld_r1_r2<2, 1>(); break;
            case 0x52: ld_r1_r2<2, 2>(); break; case 0x53: ld_r1_r2<2, 3>(); break; case 0x54: ld_r1_r2<2, 4>(); break;
            case 0x55: ld_r1_r2<2, 5>(); break; case 0x56: ld_r1_r2<2, 6>(); break; case 0x57: ld_r1_r2<2, 7>(); break;
            case 0x58: ld_r1_r2<3, 0>(); break; case 0x59: ld_r1_r2<3, 1>(); break; case 0x5a: ld_r1_r2<3, 2>(); break;
            case 0x5b: ld_r1_r2<3, 3>(); break; case 0x5c: ld_r1_r2<3, 4>(); break; case 0x5d: ld_r1_r2<3, 5>(); break;
            case 0x5e: ld_r1_r2<3, 6>(); break; case 0x5f: ld_r1_r2<3, 7>(); break; case 0x60: ld_r1_r2<4, 0>(); break;
            case 0x61: ld_r1_r2<4, 1>(); break; case 0x62: ld_r1_r2<4, 2>(); break; case 0x63: ld_r1_r2<4, 3>(); break;
            case 0x64: ld_r1_r2<4, 4>(); break; case 0x65: ld_r1_r2<4, 5>(); break; case 0x66: ld_r1_r2<4, 6>(); break;
            case 0x67: ld_r1_r2<4, 7>(); break; case 0x68: ld_r1_r2<5, 0>(); break; case 0x69: ld_r1_r2<5, 1>(); break;
            case 0x6a: ld_r1_r2<5, 2>(); break; case 0x6b: ld_r1_r2<5, 3>(); break; case 0x6c: ld_r1_r2<5, 4>(); break;
            case 0x6d: ld_r1_r2<5, 5>(); break; case 0x6e: ld_r1_r2<5, 6>(); break; case 0x6f: ld_r1_r2<5, 7>(); break;
            case 0x70: ld_r1_r2<6, 0>(); break; case 0x71: ld_r1_r2<6, 1>(); break; case 0x72: ld_r1_r2<6, 2>(); break;
            case 0x73: ld_r1_r2<6, 3>(); break; case 0x74: ld_r1_r2<6, 4>(); break; case 0x75: ld_r1_r2<6, 5>(); break;
            case 0x76: ld_r1_r2<6, 6>(); break; case 0x77: ld_r1_r2<6, 7>(); break; case 0x78: ld_r1_r2<7, 0>(); break;
            case 0x79: ld_r1_r2<7, 1>(); break; case 0x7a: ld_r1_r2<7, 2>(); break; case 0x7b: ld_r1_r2<7, 3>(); break;
            case 0x7c: ld_r1_r2<7, 4>(); break; case 0x7d: ld_r1_r2<7, 5>(); break; case 0x7e: ld_r1_r2<7, 6>(); break;
            case 0x7f: ld_r1_r2<7, 7>(); break;
            case 0xc3: registers.pc = read_u16(); break;
        }

        return cycles(op);
    }

    template <u8 r>
    void inc_r() {
        r_set<r>(r_get<r>()++);

        flags
    }

    template <u8 r>
    void dec_r() {
        r_set<r>(r_get<r>()--);

        flags
    }

    template <u8 r>
    void inc_rr() {
        rp<r>()++;
    }

    template <u8 r>
    void dec_rr() {
        rp<r>()--;
    }

    template <u8 r>
    void ldid_nn_a() {
        registers.a = mmu.read(rp3<r>());

        if constexpr (r == 2)
            registers.hl++;
        else if constexpr (r == 3)
            registers.hl--;
    }

    template <u8 r>
    void ldid_a_nn() {
        mmu.write(rp3<r>(), registers.a);

        if constexpr (r == 2)
        registers.hl++;
        else if constexpr (r == 3)
        registers.hl--;
    }

    template <u8 r>
    void ld_r_n() {
        r_set<r>(read_u8());
    }

    template <u8 p>
    void ld_rp_nn() {
        u16 nn = read_u16();
        rp<p>() = nn;
    }

    template <u8 r1, u8 r2>
    void ld_r1_r2() {
        r_set<r1>(r_get<r2>());
    }

    template <u8 p>
    u8 r_get() {
        if constexpr (p == 0)
        return registers.b;
        else if constexpr (p == 1)
        return registers.c;
        else if constexpr (p == 2)
        return registers.d;
        else if constexpr (p == 3)
        return registers.e;
        else if constexpr (p == 4)
        return registers.h;
        else if constexpr (p == 5)
        return registers.l;
        else if constexpr (p == 6)
        return mmu.read(registers.hl);
        else if constexpr (p == 7)
        return registers.a;
    }

    template <u8 p>
    void r_set(u8 value) {
        if constexpr (p == 0)
            registers.b = value;
        else if constexpr (p == 1)
            registers.c = value;
        else if constexpr (p == 2)
            registers.d = value;
        else if constexpr (p == 3)
            registers.e = value;
        else if constexpr (p == 4)
            registers.h = value;
        else if constexpr (p == 5)
            registers.l = value;
        else if constexpr (p == 6)
            mmu.write(registers.hl, value); // had to get/set bc of this guy
        else if constexpr (p == 7)
            registers.a = value;
    }

    template <u8 p>
    auto& rp() {
        if constexpr (p == 0)
            return registers.bc;
        else if constexpr (p == 1)
            return registers.de;
        else if constexpr (p == 2)
            return registers.hl;
        else if constexpr (p == 3)
            return registers.sp;
    }

    template <u8 p>
    auto& rp3() {
        if constexpr (p == 0)
            return registers.bc;
        else if constexpr (p == 1)
            return registers.de;
        else if constexpr (p == 2 || p == 3)
            return registers.hl;
    }

    u8 read_u8() {
        auto op = mmu.read(registers.pc);
        registers.pc++;
        return op;
    }

    u16 read_u16() {
        // little endian
        auto low = read_u8();
        auto high = read_u8();
        return static_cast<u16>((high << 8) | low);
    }

    static size_t cycles(u8 op) {
        return 4;
    }

};
