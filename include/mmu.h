/**
 * @file mmu.h
 * @brief Manage the memory of the gameboy.
 * @date 2020/1/8
 */

#ifndef GBE_MMU_H
#define GBE_MMU_H

#include <string>
#include "cartridge.h"


class MMU {

public:
    explicit MMU() = default;

public:

    void loadCartridge(string const & path) const noexcept {

    }

private:



};

#endif //GBE_MMU_H
