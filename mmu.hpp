#pragma once
#include <memory>
#include <utility>

class MMU {
public:
    explicit MMU(Cartrigde& cart) : cart(cart) {temp = std::vector<u8>(0xffff, 0);}

    u8 read(u16 address) const {
        switch (address) {
            case 0 ... 0x7fff:
                return cart.read(address);
            default:
                return temp[address]; // TODO
        }
    }

    void write(u16 address, u8 value) {
        switch (address) { // TODO implement the "do nothing" and so from invalid regions
            case 0 ... 0x7fff:
                cart.write(address, value);
            default:
                temp[address] = value; // TODO
        }
    }

private:
    Cartrigde& cart;
    std::vector<u8> temp; // TODO remove
};
