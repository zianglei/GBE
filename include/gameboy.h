/**
 * @file gameboy.h
 * @brief Gameboy class definition.
 * @date 2019/12/16
 */

#ifndef GBE_GAMEBOY_H
#define GBE_GAMEBOY_H


class Gameboy {

public:
    Gameboy():  {}

private:
    CPU cpu;
    GPU gpu;
    MMU mmu;
    Timer timer;
    // Sounder sounder;

};

#endif //GBE_GAMEBOY_H
