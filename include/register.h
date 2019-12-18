/**
 * @file register.h
 * @brief The gameboy register definition.
 * @date 2019/12/17
 */

#ifndef GBE_REGISTER_H
#define GBE_REGISTER_H

#include <cstdint>
/**
 * Two 8-Bits registers in Gameboy are often paired to form a 16-Bit register. Instead of using union to represent
 * a register, Register class includes two uint8_t members, corresponding to the lower 8-Bits register and the higher 8-Bits
 * register.
 */

class Register {

public:

    [[nodiscard]] uint16_t read() const { return (hi << 8u) | lo;};
    [[nodiscard]] uint8_t readLo() const { return lo; };
    [[nodiscard]] uint8_t readHi() const { return hi; };

    void write(uint16_t val) {
        hi = static_cast<uint8_t>((val >> 8u) & 0x00ffu);
        lo = static_cast<uint8_t >(val & 0x00ffu);
    };

    void writeLo(uint8_t val) { lo = val; }
    void wroteHi(uint8_t val) { hi = val; };

private:
    uint8_t lo = 0;
    uint8_t hi = 0;
};

#endif //GBE_REGISTER_H
