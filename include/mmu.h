/**
 * @file mmu.h
 * @brief Manage the memory of the gameboy.
 * @date 2020/1/8
 */

#ifndef GBE_MMU_H
#define GBE_MMU_H

#include <string>
#include <memory>
#include "cartridge.h"


class MMU {

public:
    explicit MMU() = default;

public:

    void loadCartridge(string const & path) noexcept {
        ifstream s(path);
        if (!s) {
            throw invalid_argument("The path: " + path + " is invalid!");
        }
        cartridge = std::make_unique<Cartridge>(s);

    }

private:

    std::unique_ptr<Cartridge> cartridge;


};

#endif //GBE_MMU_H
