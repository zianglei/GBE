/**
 * @file cpu.h
 * @brief The gameboy processor class definition.
 * @date 2019/12/17
 */

#ifndef GBE_CPU_H
#define GBE_CPU_H

#include <stdexcept>
#include <register.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;

typedef uint32_t Cycle;

class CPU {

public:

    CPU() = default;

    /**
     * Read the value from the corresponding register.
     * @param name register name
     * @throw If the register name is invalid, throw `out_of_range` exception.
     * @return The register value
     */
    BYTE read_register(char name) {
        switch (name) {
            case 'A': return AF.readHi();
            case 'F': return AF.readLo();
            case 'B': return BC.readHi();
            case 'C': return BC.readLo();
            case 'D': return DE.readHi();
            case 'E': return DE.readLo();
            case 'H': return HL.readHi();
            case 'L': return HL.readLo();
            default:
                throw std::out_of_range("Invalid register");
        }
    }

    Cycle tick() {

    }




private:

    Register AF;
    Register BC;
    Register DE;
    Register HL;

    WORD pc = 0;

};

#endif //GBE_CPU_H
