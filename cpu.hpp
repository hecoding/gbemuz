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

enum class Flag {
    Zero = 1 << 7,
    Negative = 1 << 6,
    HalfCarry = 1 << 5,
    Carry = 1 << 4,
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
    bool interrupt_enabled = true;

    void print_debug(u8 op) const {
        // std::setw(2) << std::setfill('0')
        setbuf(stdout, nullptr);
        std::cout
        << "A: " << std::hex << std::uppercase << std::noboolalpha << (uint) registers.a
        << " B: " << (uint) registers.b
        << " C: " << (uint) registers.c
        << " D: " << (uint) registers.d << std::endl;
        std::cout
        << "E: " << (uint) registers.e
        << " F: " << (uint) registers.f
        << " H: " << (uint) registers.h
        << " L: " << (uint) registers.l << std::endl;
        std::cout
        << "PC: " << (uint) registers.pc - 1
        << " SP: " << (uint) registers.sp << std::endl;
        std::cout
        << "Z: " << read_flag(Flag::Zero)
        << " N: " << read_flag(Flag::Negative)
        << " H: " << read_flag(Flag::HalfCarry)
        << " C: " << read_flag(Flag::Carry) << std::endl;
        std::cout << "Next instruction to execute: " << (uint) op;
        if (op == 0xcb)
            std::cout << " " << (uint) mmu.read(registers.pc);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "All interrupts Enabled: " << std::boolalpha << interrupt_enabled << std::endl;
        std::cout << "halted: " << halted << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    size_t exec() {
        u8 op = read_u8();
//        print_debug(op);

        if (op == 0xcb)
            exec_prefixed(read_u8());
        else
            exec_regular(op);

        return cycles(op);
    }

    void exec_regular(u8 op) {
        switch (op) {
            case 0x00: break;
            case 0x01: ld_rp_nn<0>(); break;
            case 0x02: ldid_a_nn<0>(); break;
            case 0x03: inc_rr<0>(); break;
            case 0x04: inc_r<0>(); break;
            case 0x05: dec_r<0>(); break;
            case 0x06: ld_r_n<0>(); break;
            case 0x07: rot<0, 7>(); set_flag(Flag::Zero, false); break;
            case 0x08: ld_nn_sp(); break;
            case 0x09: add_hl_n<0>(); break;
            case 0x0a: ldid_nn_a<0>(); break;
            case 0x0b: dec_rr<0>(); break;
            case 0x0c: inc_r<1>(); break;
            case 0x0d: dec_r<1>(); break;
            case 0x0e: ld_r_n<1>(); break;
            case 0x0f: rot<1, 7>(); set_flag(Flag::Zero, false); break;
            case 0x10: stop(); break;
            case 0x11: ld_rp_nn<1>(); break;
            case 0x12: ldid_a_nn<1>(); break;
            case 0x13: inc_rr<1>(); break;
            case 0x14: inc_r<2>(); break;
            case 0x15: dec_r<2>(); break;
            case 0x16: ld_r_n<2>(); break;
            case 0x17: rot<2, 7>(); set_flag(Flag::Zero, false); break;
            case 0x18: jr_n(); break;
            case 0x19: add_hl_n<1>(); break;
            case 0x1a: ldid_nn_a<1>(); break;
            case 0x1b: dec_rr<1>(); break;
            case 0x1c: inc_r<3>(); break;
            case 0x1d: dec_r<3>(); break;
            case 0x1e: ld_r_n<3>(); break;
            case 0x1f: rot<3, 7>(); set_flag(Flag::Zero, false); break;
            case 0x20: j_cc_n<0, true>(); break;
            case 0x21: ld_rp_nn<2>(); break;
            case 0x22: ldid_a_nn<2>(); break;
            case 0x23: inc_rr<2>(); break;
            case 0x24: inc_r<4>(); break;
            case 0x25: dec_r<4>(); break;
            case 0x26: ld_r_n<4>(); break;
            case 0x27: daa(); break;
            case 0x28: j_cc_n<1, true>(); break;
            case 0x29: add_hl_n<2>(); break;
            case 0x2a: ldid_nn_a<2>(); break;
            case 0x2b: dec_rr<2>(); break;
            case 0x2c: inc_r<5>(); break;
            case 0x2d: dec_r<5>(); break;
            case 0x2e: ld_r_n<5>(); break;
            case 0x2f: cpl(); break;
            case 0x30: j_cc_n<2, true>(); break;
            case 0x31: ld_rp_nn<3>(); break;
            case 0x32: ldid_a_nn<3>(); break;
            case 0x33: inc_rr<3>(); break;
            case 0x34: inc_r<6>(); break;
            case 0x35: dec_r<6>(); break;
            case 0x36: ld_r_n<6>(); break;
            case 0x37: scf(); break;
            case 0x38: j_cc_n<3, true>(); break;
            case 0x39: add_hl_n<3>(); break;
            case 0x3a: ldid_nn_a<3>(); break;
            case 0x3b: dec_rr<3>(); break;
            case 0x3c: inc_r<7>(); break;
            case 0x3d: dec_r<7>(); break;
            case 0x3e: ld_r_n<7>(); break;
            case 0x3f: ccf(); break;
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
            case 0x80: add<0>(); break; case 0x81: add<1>(); break; case 0x82: add<2>(); break;
            case 0x83: add<3>(); break; case 0x84: add<4>(); break; case 0x85: add<5>(); break;
            case 0x86: add<6>(); break; case 0x87: add<7>(); break; case 0x88: adc<0>(); break;
            case 0x89: adc<1>(); break; case 0x8a: adc<2>(); break; case 0x8b: adc<3>(); break;
            case 0x8c: adc<4>(); break; case 0x8d: adc<5>(); break; case 0x8e: adc<6>(); break;
            case 0x8f: adc<7>(); break; case 0x90: sub<0>(); break; case 0x91: sub<1>(); break;
            case 0x92: sub<2>(); break; case 0x93: sub<3>(); break; case 0x94: sub<4>(); break;
            case 0x95: sub<5>(); break; case 0x96: sub<6>(); break; case 0x97: sub<7>(); break;
            case 0x98: sbc<0>(); break; case 0x99: sbc<1>(); break; case 0x9a: sbc<2>(); break;
            case 0x9b: sbc<3>(); break; case 0x9c: sbc<4>(); break; case 0x9d: sbc<5>(); break;
            case 0x9e: sbc<6>(); break; case 0x9f: sbc<7>(); break; case 0xa0: and_<0>(); break;
            case 0xa1: and_<1>(); break; case 0xa2: and_<2>(); break; case 0xa3: and_<3>(); break;
            case 0xa4: and_<4>(); break; case 0xa5: and_<5>(); break; case 0xa6: and_<6>(); break;
            case 0xa7: and_<7>(); break; case 0xa8: xor_<0>(); break; case 0xa9: xor_<1>(); break;
            case 0xaa: xor_<2>(); break; case 0xab: xor_<3>(); break; case 0xac: xor_<4>(); break;
            case 0xad: xor_<5>(); break; case 0xae: xor_<6>(); break; case 0xaf: xor_<7>(); break;
            case 0xb0: or_<0>(); break; case 0xb1: or_<1>(); break; case 0xb2: or_<2>(); break;
            case 0xb3: or_<3>(); break; case 0xb4: or_<4>(); break; case 0xb5: or_<5>(); break;
            case 0xb6: or_<6>(); break; case 0xb7: or_<7>(); break; case 0xb8: cp<0>(); break;
            case 0xb9: cp<1>(); break; case 0xba: cp<2>(); break; case 0xbb: cp<3>(); break;
            case 0xbc: cp<4>(); break; case 0xbd: cp<5>(); break; case 0xbe: cp<6>(); break; case 0xbf: cp<7>(); break;
            case 0xc0: ret_cc<0>(); break;
            case 0xc1: pop_nn<0>(); break;
            case 0xc2: j_cc_n<0>(); break;
            case 0xc3: jp_nn(); break;
            case 0xc4: call_cc_nn<0>(); break;
            case 0xc5: push_nn<0>(); break;
            case 0xc6: add<8>(); break;
            case 0xc7: rst<0>(); break;
            case 0xc8: ret_cc<1>(); break;
            case 0xc9: ret(); break;
            case 0xca: j_cc_n<1>(); break;
            case 0xcc: call_cc_nn<1>(); break;
            case 0xcd: call_nn(); break;
            case 0xce: adc<8>(); break;
            case 0xcf: rst<1>(); break;
            case 0xd0: ret_cc<2>(); break;
            case 0xd1: pop_nn<1>(); break;
            case 0xd2: j_cc_n<2>(); break;
            case 0xd4: call_cc_nn<2>(); break;
            case 0xd5: push_nn<1>(); break;
            case 0xd6: sub<8>(); break;
            case 0xd7: rst<2>(); break;
            case 0xd8: ret_cc<3>(); break;
            case 0xd9: reti(); break;
            case 0xda: j_cc_n<3>(); break;
            case 0xdc: call_cc_nn<3>(); break;
            case 0xde: sbc<8>(); break;
            case 0xdf: rst<3>(); break;
            case 0xe2: ld_c_a(); break;
            case 0xe0: ldh_n_a(); break;
            case 0xe1: pop_nn<2>(); break;
            case 0xe5: push_nn<2>(); break;
            case 0xe6: and_<8>(); break;
            case 0xe7: rst<4>(); break;
            case 0xe8: add_sp_n(); break;
            case 0xe9: jp_hl(); break;
            case 0xea: ld_nn_a(); break;
            case 0xee: xor_<8>(); break;
            case 0xef: rst<5>(); break;
            case 0xf0: ldh_a_n(); break;
            case 0xf1: pop_nn<3>(); break;
            case 0xf6: or_<8>(); break;
            case 0xf7: rst<6>(); break;
            case 0xf8: ld_hl_sp_n(); break;
            case 0xf9: ld_sp_hl(); break;
            case 0xfa: ld_a_nn(); break;
            case 0xf2: ld_a_c(); break;
            case 0xf3: di(); break;
            case 0xf5: push_nn<3>(); break;
            case 0xfb: ei(); break;
            case 0xfe: cp<8>(); break;
            case 0xff: rst<7>(); break;
            default:
                std::cout << "unimplemented " << op;
        }
    }

    void exec_prefixed(u8 op) {
        switch (op) {
            case 0x00: rot<0, 0>() ;break; case 0x01: rot<0, 1>() ;break; case 0x02: rot<0, 2>() ;break;
            case 0x03: rot<0, 3>() ;break; case 0x04: rot<0, 4>() ;break; case 0x05: rot<0, 5>() ;break;
            case 0x06: rot<0, 6>() ;break; case 0x07: rot<0, 7>() ;break; case 0x08: rot<1, 0>() ;break;
            case 0x09: rot<1, 1>() ;break; case 0x0a: rot<1, 2>() ;break; case 0x0b: rot<1, 3>() ;break;
            case 0x0c: rot<1, 4>() ;break; case 0x0d: rot<1, 5>() ;break; case 0x0e: rot<1, 6>() ;break;
            case 0x0f: rot<1, 7>() ;break; case 0x10: rot<2, 0>() ;break; case 0x11: rot<2, 1>() ;break;
            case 0x12: rot<2, 2>() ;break; case 0x13: rot<2, 3>() ;break; case 0x14: rot<2, 4>() ;break;
            case 0x15: rot<2, 5>() ;break; case 0x16: rot<2, 6>() ;break; case 0x17: rot<2, 7>() ;break;
            case 0x18: rot<3, 0>() ;break; case 0x19: rot<3, 1>() ;break; case 0x1a: rot<3, 2>() ;break;
            case 0x1b: rot<3, 3>() ;break; case 0x1c: rot<3, 4>() ;break; case 0x1d: rot<3, 5>() ;break;
            case 0x1e: rot<3, 6>() ;break; case 0x1f: rot<3, 7>() ;break; case 0x20: rot<4, 0>() ;break;
            case 0x21: rot<4, 1>() ;break; case 0x22: rot<4, 2>() ;break; case 0x23: rot<4, 3>() ;break;
            case 0x24: rot<4, 4>() ;break; case 0x25: rot<4, 5>() ;break; case 0x26: rot<4, 6>() ;break;
            case 0x27: rot<4, 7>() ;break; case 0x28: rot<5, 0>() ;break; case 0x29: rot<5, 1>() ;break;
            case 0x2a: rot<5, 2>() ;break; case 0x2b: rot<5, 3>() ;break; case 0x2c: rot<5, 4>() ;break;
            case 0x2d: rot<5, 5>() ;break; case 0x2e: rot<5, 6>() ;break; case 0x2f: rot<5, 7>() ;break;
            case 0x30: rot<6, 0>() ;break; case 0x31: rot<6, 1>() ;break; case 0x32: rot<6, 2>() ;break;
            case 0x33: rot<6, 3>() ;break; case 0x34: rot<6, 4>() ;break; case 0x35: rot<6, 5>() ;break;
            case 0x36: rot<6, 6>() ;break; case 0x37: rot<6, 7>() ;break; case 0x38: rot<7, 0>() ;break;
            case 0x39: rot<7, 1>() ;break; case 0x3a: rot<7, 2>() ;break; case 0x3b: rot<7, 3>() ;break;
            case 0x3c: rot<7, 4>() ;break; case 0x3d: rot<7, 5>() ;break; case 0x3e: rot<7, 6>() ;break;
            case 0x3f: rot<7, 7>() ;break;
            case 0x40: bit<0, 0>() ;break; case 0x41: bit<0, 1>() ;break; case 0x42: bit<0, 2>() ;break;
            case 0x43: bit<0, 3>() ;break; case 0x44: bit<0, 4>() ;break; case 0x45: bit<0, 5>() ;break;
            case 0x46: bit<0, 6>() ;break; case 0x47: bit<0, 7>() ;break; case 0x48: bit<1, 0>() ;break;
            case 0x49: bit<1, 1>() ;break; case 0x4a: bit<1, 2>() ;break; case 0x4b: bit<1, 3>() ;break;
            case 0x4c: bit<1, 4>() ;break; case 0x4d: bit<1, 5>() ;break; case 0x4e: bit<1, 6>() ;break;
            case 0x4f: bit<1, 7>() ;break; case 0x50: bit<2, 0>() ;break; case 0x51: bit<2, 1>() ;break;
            case 0x52: bit<2, 2>() ;break; case 0x53: bit<2, 3>() ;break; case 0x54: bit<2, 4>() ;break;
            case 0x55: bit<2, 5>() ;break; case 0x56: bit<2, 6>() ;break; case 0x57: bit<2, 7>() ;break;
            case 0x58: bit<3, 0>() ;break; case 0x59: bit<3, 1>() ;break; case 0x5a: bit<3, 2>() ;break;
            case 0x5b: bit<3, 3>() ;break; case 0x5c: bit<3, 4>() ;break; case 0x5d: bit<3, 5>() ;break;
            case 0x5e: bit<3, 6>() ;break; case 0x5f: bit<3, 7>() ;break; case 0x60: bit<4, 0>() ;break;
            case 0x61: bit<4, 1>() ;break; case 0x62: bit<4, 2>() ;break; case 0x63: bit<4, 3>() ;break;
            case 0x64: bit<4, 4>() ;break; case 0x65: bit<4, 5>() ;break; case 0x66: bit<4, 6>() ;break;
            case 0x67: bit<4, 7>() ;break; case 0x68: bit<5, 0>() ;break; case 0x69: bit<5, 1>() ;break;
            case 0x6a: bit<5, 2>() ;break; case 0x6b: bit<5, 3>() ;break; case 0x6c: bit<5, 4>() ;break;
            case 0x6d: bit<5, 5>() ;break; case 0x6e: bit<5, 6>() ;break; case 0x6f: bit<5, 7>() ;break;
            case 0x70: bit<6, 0>() ;break; case 0x71: bit<6, 1>() ;break; case 0x72: bit<6, 2>() ;break;
            case 0x73: bit<6, 3>() ;break; case 0x74: bit<6, 4>() ;break; case 0x75: bit<6, 5>() ;break;
            case 0x76: bit<6, 6>() ;break; case 0x77: bit<6, 7>() ;break; case 0x78: bit<7, 0>() ;break;
            case 0x79: bit<7, 1>() ;break; case 0x7a: bit<7, 2>() ;break; case 0x7b: bit<7, 3>() ;break;
            case 0x7c: bit<7, 4>() ;break; case 0x7d: bit<7, 5>() ;break; case 0x7e: bit<7, 6>() ;break;
            case 0x7f: bit<7, 7>() ;break;
            case 0x80: res<0, 0>() ;break; case 0x81: res<0, 1>() ;break; case 0x82: res<0, 2>() ;break;
            case 0x83: res<0, 3>() ;break; case 0x84: res<0, 4>() ;break; case 0x85: res<0, 5>() ;break;
            case 0x86: res<0, 6>() ;break; case 0x87: res<0, 7>() ;break; case 0x88: res<1, 0>() ;break;
            case 0x89: res<1, 1>() ;break; case 0x8a: res<1, 2>() ;break; case 0x8b: res<1, 3>() ;break;
            case 0x8c: res<1, 4>() ;break; case 0x8d: res<1, 5>() ;break; case 0x8e: res<1, 6>() ;break;
            case 0x8f: res<1, 7>() ;break; case 0x90: res<2, 0>() ;break; case 0x91: res<2, 1>() ;break;
            case 0x92: res<2, 2>() ;break; case 0x93: res<2, 3>() ;break; case 0x94: res<2, 4>() ;break;
            case 0x95: res<2, 5>() ;break; case 0x96: res<2, 6>() ;break; case 0x97: res<2, 7>() ;break;
            case 0x98: res<3, 0>() ;break; case 0x99: res<3, 1>() ;break; case 0x9a: res<3, 2>() ;break;
            case 0x9b: res<3, 3>() ;break; case 0x9c: res<3, 4>() ;break; case 0x9d: res<3, 5>() ;break;
            case 0x9e: res<3, 6>() ;break; case 0x9f: res<3, 7>() ;break; case 0xa0: res<4, 0>() ;break;
            case 0xa1: res<4, 1>() ;break; case 0xa2: res<4, 2>() ;break; case 0xa3: res<4, 3>() ;break;
            case 0xa4: res<4, 4>() ;break; case 0xa5: res<4, 5>() ;break; case 0xa6: res<4, 6>() ;break;
            case 0xa7: res<4, 7>() ;break; case 0xa8: res<5, 0>() ;break; case 0xa9: res<5, 1>() ;break;
            case 0xaa: res<5, 2>() ;break; case 0xab: res<5, 3>() ;break; case 0xac: res<5, 4>() ;break;
            case 0xad: res<5, 5>() ;break; case 0xae: res<5, 6>() ;break; case 0xaf: res<5, 7>() ;break;
            case 0xb0: res<6, 0>() ;break; case 0xb1: res<6, 1>() ;break; case 0xb2: res<6, 2>() ;break;
            case 0xb3: res<6, 3>() ;break; case 0xb4: res<6, 4>() ;break; case 0xb5: res<6, 5>() ;break;
            case 0xb6: res<6, 6>() ;break; case 0xb7: res<6, 7>() ;break; case 0xb8: res<7, 0>() ;break;
            case 0xb9: res<7, 1>() ;break; case 0xba: res<7, 2>() ;break; case 0xbb: res<7, 3>() ;break;
            case 0xbc: res<7, 4>() ;break; case 0xbd: res<7, 5>() ;break; case 0xbe: res<7, 6>() ;break;
            case 0xbf: res<7, 7>() ;break;
            case 0xc0: set<0, 0>() ;break; case 0xc1: set<0, 1>() ;break; case 0xc2: set<0, 2>() ;break;
            case 0xc3: set<0, 3>() ;break; case 0xc4: set<0, 4>() ;break; case 0xc5: set<0, 5>() ;break;
            case 0xc6: set<0, 6>() ;break; case 0xc7: set<0, 7>() ;break; case 0xc8: set<1, 0>() ;break;
            case 0xc9: set<1, 1>() ;break; case 0xca: set<1, 2>() ;break; case 0xcb: set<1, 3>() ;break;
            case 0xcc: set<1, 4>() ;break; case 0xcd: set<1, 5>() ;break; case 0xce: set<1, 6>() ;break;
            case 0xcf: set<1, 7>() ;break; case 0xd0: set<2, 0>() ;break; case 0xd1: set<2, 1>() ;break;
            case 0xd2: set<2, 2>() ;break; case 0xd3: set<2, 3>() ;break; case 0xd4: set<2, 4>() ;break;
            case 0xd5: set<2, 5>() ;break; case 0xd6: set<2, 6>() ;break; case 0xd7: set<2, 7>() ;break;
            case 0xd8: set<3, 0>() ;break; case 0xd9: set<3, 1>() ;break; case 0xda: set<3, 2>() ;break;
            case 0xdb: set<3, 3>() ;break; case 0xdc: set<3, 4>() ;break; case 0xdd: set<3, 5>() ;break;
            case 0xde: set<3, 6>() ;break; case 0xdf: set<3, 7>() ;break; case 0xe0: set<4, 0>() ;break;
            case 0xe1: set<4, 1>() ;break; case 0xe2: set<4, 2>() ;break; case 0xe3: set<4, 3>() ;break;
            case 0xe4: set<4, 4>() ;break; case 0xe5: set<4, 5>() ;break; case 0xe6: set<4, 6>() ;break;
            case 0xe7: set<4, 7>() ;break; case 0xe8: set<5, 0>() ;break; case 0xe9: set<5, 1>() ;break;
            case 0xea: set<5, 2>() ;break; case 0xeb: set<5, 3>() ;break; case 0xec: set<5, 4>() ;break;
            case 0xed: set<5, 5>() ;break; case 0xee: set<5, 6>() ;break; case 0xef: set<5, 7>() ;break;
            case 0xf0: set<6, 0>() ;break; case 0xf1: set<6, 1>() ;break; case 0xf2: set<6, 2>() ;break;
            case 0xf3: set<6, 3>() ;break; case 0xf4: set<6, 4>() ;break; case 0xf5: set<6, 5>() ;break;
            case 0xf6: set<6, 6>() ;break; case 0xf7: set<6, 7>() ;break; case 0xf8: set<7, 0>() ;break;
            case 0xf9: set<7, 1>() ;break; case 0xfa: set<7, 2>() ;break; case 0xfb: set<7, 3>() ;break;
            case 0xfc: set<7, 4>() ;break; case 0xfd: set<7, 5>() ;break; case 0xfe: set<7, 6>() ;break;
            case 0xff: set<7, 7>() ;break;
            default:
                std::cout << "unimplemented " << op;
        }
    }

    template<u8 n_bit, u8 r>
    void bit() {
        set_flag(Flag::Zero, !get_register_bit<n_bit, r>());
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, true);
    }

    template<u8 n_bit, u8 r>
    void res() {
        set_register_bit<n_bit, r, false>();
    }

    template<u8 n_bit, u8 r>
    void set() {
        set_register_bit<n_bit, r>();
    }

    template<u8 bit, u8 r>
    inline bool get_register_bit() {
        return r_get<r>() & (1 << bit);
    }

    template<u8 bit, u8 r, bool value = true>
    void set_register_bit() {
        u8 reg = r_get<r>();
        u8 mask = 1 << bit;
        if (value)
            reg |= mask;
        else
            reg &= ~mask;

        r_set<r>(reg);
    }

    void cpl() {
        registers.a = ~registers.a;
        set_flag(Flag::Negative, true);
        set_flag(Flag::HalfCarry, true);
    }

    void scf() {
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, true);
    }

    void ccf() {
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, !read_flag(Flag::Carry));
    }

    void stop() {
//        throw std::runtime_error("Not implemented! (0x10 stop)");
    }

    template<u8 n>
    void rst() {
        push(registers.pc);
        registers.pc = 8 * n;
    }

    void daa() {
        u8 reg = registers.a;
        u16 correction = read_flag(Flag::Carry) ? 0x60 : 0x00;

        if (read_flag(Flag::HalfCarry) || (!read_flag(Flag::Negative) && ((reg & 0x0F) > 9)))
            correction |= 0x06;

        if (read_flag(Flag::Carry) || (!read_flag(Flag::Negative) && (reg > 0x99)))
            correction |= 0x60;

        if (read_flag(Flag::Negative))
            reg = static_cast<u8>(reg - correction);
        else
            reg = static_cast<u8>(reg + correction);

        set_flag(Flag::Zero, reg == 0);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, ((correction << 2) & 0x100) != 0);

        registers.a = static_cast<u8>(reg);
    }

    void ld_hl_sp_n() {
        s8 n = read_s8();

        set_flag(Flag::Zero, false);
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_signed_carry(4, registers.sp, n));
        set_flag(Flag::Carry, CPU::is_signed_carry(8, registers.sp, n));

        registers.hl = registers.sp + n;
    }

    void ld_sp_hl() {
        registers.sp = registers.hl;
    }

    void ld_nn_sp() {
        u16 nn = read_u16();
        mmu.write(nn, registers.sp & 0x00FF);
        mmu.write(nn + 1, (registers.sp & 0xFF00) >> 8);
    }

    void jp_hl() {
        registers.pc = registers.hl;
    }

    template<u8 r>
    void add_hl_n() {
        u16 n = rp<r>();
        u32 x = registers.hl + n;

        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_carry_from_bit(11, registers.hl, n));
        set_flag(Flag::Carry, CPU::is_carry_from_bit(15, registers.hl, n));

        registers.hl = static_cast<u16>(x);
    }

    void add_sp_n() {
        s8 n = read_s8();

        set_flag(Flag::Zero, false);
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_signed_carry(4, registers.sp, n));
        set_flag(Flag::Carry, CPU::is_signed_carry(8, registers.sp, n));

        registers.sp += n;
    }

    template<u8 op, u8 r>
    void rot() {
        u8 n = r_get<r>();
        bool carry = false;
        u8 result = 0;

        if constexpr (op == 0) { // rlc
            carry = read_bit(7, n);
            result = (n << 1) | carry;
        } else if constexpr (op == 1) { // rrc
            carry = read_bit(0, n);
            result = (n >> 1) | (0x80 * carry);
        } else if constexpr (op == 2) { // rl
            carry = read_bit(7, n);
            result = (n << 1) | read_flag(Flag::Carry);
        } else if constexpr (op == 3) { // rr
            carry = read_bit(0, n);
            result = (n >> 1) | (0x80 * read_flag(Flag::Carry));
        } else if constexpr (op == 4) { // sla
            carry = read_bit(7, n);
            result = n << 1;
        } else if constexpr (op == 5) { // sra
            carry = read_bit(0, n);
            result = (n >> 1) | (0x80 & n);
        } else if constexpr (op == 6) { // swap
            carry = false;
            result = (n << 4) | (n >> 4);
        } else if constexpr (op == 7) { // srl
            carry = read_bit(0, n);
            result = n >> 1;
        }

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, carry);

        r_set<r>(result);
    }

    static inline bool read_bit(u8 bit, u8 value) {
        return (value & (1 << bit)) != 0;
    }

    // alu
    template<u8 r>
    void add() {
        u8 n = r_get<r>();
        u16 x = registers.a + n;
        u8 result = static_cast<u8>(x);

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_carry_from_bit(3, registers.a, n));
        set_flag(Flag::Carry, CPU::is_carry_from_bit(7, registers.a, n));

        registers.a = result;
    }

    template<u8 r>
    void adc() {
        u8 n = r_get<r>();
        bool carry = read_flag(Flag::Carry);
        u16 x = registers.a + n + carry;
        u8 result = static_cast<u8>(x);

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_carry_from_bit(3, registers.a, n, carry));
        set_flag(Flag::Carry, CPU::is_carry_from_bit(7, registers.a, n, carry));

        registers.a = result;
    }

    template<u8 r>
    void sub() {
        u8 n = r_get<r>();
        u8 result = registers.a - n;

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, true);
        set_flag(Flag::HalfCarry, CPU::is_no_borrow_from_bit(4, registers.a, n));
        set_flag(Flag::Carry, CPU::is_no_borrow_from_bit(8, registers.a, n));

        registers.a = result;
    }

    template<u8 r>
    void sbc() {
        u8 n = r_get<r>();
        bool carry = read_flag(Flag::Carry);
        u8 result = registers.a - n - carry;

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, true);
        set_flag(Flag::HalfCarry, CPU::is_no_borrow_from_bit(4, registers.a, n, carry));
        set_flag(Flag::Carry, CPU::is_no_borrow_from_bit(8, registers.a, n, carry));

        registers.a = result;
    }

    template<u8 r>
    void and_() {
        registers.a &= r_get<r>();

        set_flag(Flag::Zero, CPU::is_result_zero(registers.a));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, true);
        set_flag(Flag::Carry, false);
    }

    template<u8 r>
    void xor_() {
        registers.a ^= r_get<r>();

        set_flag(Flag::Zero, CPU::is_result_zero(registers.a));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, false);
    }

    template<u8 r>
    void or_() {
        registers.a |= r_get<r>();

        set_flag(Flag::Zero, CPU::is_result_zero(registers.a));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, false);
        set_flag(Flag::Carry, false);
    }

    template<u8 r>
    void cp() {
        u8 n = r_get<r>();
        set_flag(Flag::Zero, registers.a == n);
        set_flag(Flag::Negative, true);
        set_flag(Flag::HalfCarry, CPU::is_no_borrow_from_bit(4, registers.a, n));
        set_flag(Flag::Carry, CPU::is_no_borrow_from_bit(8, registers.a, n));
    }

    template<u8 p>
    void push_nn() {
        push(rp2<p>());
    }

    template<u8 p>
    void pop_nn() {
        if constexpr(p == 3) // ignore 0-3 bit in AF
            rp2<p>() = pop() & 0xFFF0;
        else
            rp2<p>() = pop();
    }

    void ret() {
        registers.pc = pop();
    }

    template<u8 c>
    void ret_cc() {
        if (condition_checks<c>())
            registers.pc = pop();
    }

    void reti() {
        registers.pc = pop();
        interrupt_enabled = true;
    }

    void push(u16 n) {
        mmu.write(registers.sp - 1, (n & 0xFF00) >> 8);
        mmu.write(registers.sp - 2, n & 0x00FF);
        registers.sp -= 2;
    }

    u16 pop() {
        u16 n = (mmu.read(registers.sp + 1) << 8) | mmu.read(registers.sp);
        registers.sp += 2;
        return n;
    }

    void call_nn() {
        u16 nn = read_u16(); // get pc updated before saving it
        push(registers.pc);
        registers.pc = nn;
    }

    template<u8 c>
    void call_cc_nn() {
        u16 nn = read_u16();

        if (condition_checks<c>()) {
            push(registers.pc);
            registers.pc = nn;
        }
    }

    void ldh_n_a() {
        mmu.write(0xff00 + read_u8(), registers.a);
    }

    void ldh_a_n() {
        registers.a = mmu.read(0xff00 + read_u8());
    }

    void ld_a_c() {
        registers.a = mmu.read(0xff00 + registers.c);
    }

    void ld_c_a() {
        mmu.write(0xff00 + registers.c, registers.a);
    }

    void ld_nn_a() {
        mmu.write(read_u16(), registers.a);
    }

    void ld_a_nn() {
        registers.a = mmu.read(read_u16());
    }

    void di() {
        interrupt_enabled = false;
    }

    void ei() {
        interrupt_enabled = true;
    }

    void jp_nn() {
        registers.pc = read_u16();
    }

    void jr_n() {
        registers.pc += read_s8();
    }

    template<u8 c, bool relative = false>
    void j_cc_n() {
        std::conditional_t<relative, s8, u8> n;
        if constexpr(relative)
            n = read_s8();
        else
            n = read_u8();

        if (condition_checks<c>())
            registers.pc += n;
    }

    template<u8 c>
    bool condition_checks() {
        if constexpr (c == 0)
            return !read_flag(Flag::Zero);
        else if constexpr (c == 1)
            return read_flag(Flag::Zero);
        else if constexpr (c == 2)
            return !read_flag(Flag::Carry);
        else if constexpr (c == 3)
            return read_flag(Flag::Carry);
    }

    inline bool read_flag(Flag f) const {
        return registers.f & static_cast<u8>(f);
    }

    template <u8 r>
    void inc_r() {
        u8 result = r_get<r>() + 1;

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, false);
        set_flag(Flag::HalfCarry, CPU::is_carry_from_bit(3, r_get<r>(), 1));

        r_set<r>(result);
    }

    template <u8 r>
    void dec_r() {
        u8 result = r_get<r>() - 1;

        set_flag(Flag::Zero, CPU::is_result_zero(result));
        set_flag(Flag::Negative, true);
        set_flag(Flag::HalfCarry, CPU::is_no_borrow_from_bit(4, r_get<r>()));

        r_set<r>(result);
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
        else if constexpr (p == 8)
            return read_u8();
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
    auto& rp2() {
        if constexpr (p == 0)
            return registers.bc;
        else if constexpr (p == 1)
            return registers.de;
        else if constexpr (p == 2)
            return registers.hl;
        else if constexpr (p == 3)
            return registers.af;
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

    void set_flag(Flag f, bool b) {
        if (b)
            registers.f |= static_cast<u8>(f);
        else
            registers.f &= ~static_cast<u8>(f);
    }

    static inline bool is_result_zero(u8 b) {
        return b == 0;
    }

    template<typename T = u8, typename S = u8>
    static inline bool is_carry_from_bit(u8 bit, T op1, S op2, bool carry = false) {
        // bit: counting right to left and starting at 0
        S mask = (1 << (bit + 1)) - 1;
        return ((op1 & mask) + (op2 & mask) + carry) > mask;
    }

    static inline bool is_no_borrow_from_bit(u16 bit, u8 op1, u8 op2 = 1, bool carry = false) {
        // bit: counting right to left and starting at 0
        u8 mask = static_cast<u8>((1 << bit) - 1);
        return ((op1 & mask) - (op2 & mask) - carry) < 0;
    }

    static inline bool is_signed_carry(u8 bit, u16 r, s8 n) {
        u16 mask = 1 << bit;
        return ((r ^ n ^ ((r + n) & 0xFFFF)) & mask) == mask;
    }

    u8 read_u8() {
        auto op = mmu.read(registers.pc);
        registers.pc++;
        return op;
    }

    s8 read_s8() {
        auto n = read_u8();
        return static_cast<s8>(n);
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
