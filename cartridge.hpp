#pragma once
#include <fstream>
#include <vector>

class Cartrigde {
private:
    const std::vector<u8> rom;

public:
    explicit Cartrigde(const std::string& filepath) : rom(load_file(filepath)) {
    }

    static std::vector<u8> load_file(const std::string& filepath) {
        std::ifstream ifs(filepath, std::ios::binary|std::ios::ate);

        if(!ifs)
            throw std::runtime_error(filepath + ": " + std::strerror(errno));

        auto end = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        auto size = std::size_t(end - ifs.tellg());

        if(size == 0) // avoid undefined behavior
            return {};

        std::vector<u8> buffer(size);

        if(!ifs.read((char*)buffer.data(), buffer.size()))
            throw std::runtime_error(filepath + ": " + std::strerror(errno));

        return buffer;
    }

    std::string title() const {
        return {rom.begin() + 0x134, rom.begin() + 0x142};
    }
};
