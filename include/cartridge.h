/**
 * @file cartridge.h
 * @brief The cartridge class definition
 * @date 2019/12/18
 */

#ifndef GBE_CARTRIDGE_H
#define GBE_CARTRIDGE_H

#include <string>
#include <array>
#include <fstream>
#include <stdexcept>

using namespace std;

class CartridgeHeader {

public:
    explicit CartridgeHeader(ifstream& cartridge) {
        if (!cartridge) {
            throw invalid_argument("The cartridge is not opened");
        }

        cartridge.seekg(0);
        cartridge.read(reinterpret_cast<char*>(entryPoint.data()), entryPoint.size());
        cartridge.read(reinterpret_cast<char*>(logo.data()), logo.size());

        char contents[CHECKSUM_SCALE] = {}; // only used to calculate checksum.
        cartridge.read(contents, CHECKSUM_SCALE);
        cartridge.seekg(cartridge.gcount() - CHECKSUM_SCALE);

        // fixme: Only support non-CGB mode for now.
        cartridge.width(16);
        cartridge >> title;
        cartridge.width(2);
        cartridge >> newLicenseCode;

        cartridge.width(1);
        cartridge >> sgbFlag >> cartridgeType >> romSize >> ramSize
                >> destinationCode >> oldLicenseCode >> maskRomVerNumber
                >> headerChecksum >> globalChecksum;

        if (!isValid(contents)) {
            throw invalid_argument("The cartridge header checksum is wrong!");
        }
    }

private:
                                              // byte position
    array<char , 4> entryPoint{};              // 0100h-0103h
    array<char, 48> logo{};                   // 0104h-0133h

    string title = "";
    string newLicenseCode = "";

    char sgbFlag = 0;
    char cartridgeType = 0;
    char romSize = 0;
    char ramSize = 0;
    char destinationCode = 0;
    char oldLicenseCode = 0;
    char maskRomVerNumber = 0;
    char headerChecksum = 0;
    uint16_t globalChecksum = 0;              // 014Eh-014Fh

private:
    static constexpr int CHECKSUM_SCALE = 25;

private:
    /**
     * Calculate the 8-bit checksum:
     *   x=0:FOR i=0134h TO 014Ch: x=x-MEM[i]-1:NEXT
     * @return true if the calculated checksum is equal to the checksum in the header.
     */
    bool isValid(char (&contents)[CHECKSUM_SCALE]) noexcept {
        uint8_t x = 0;
        for (uint8_t b: contents) {
            x -= b + 1;
        }
        return x == headerChecksum;
    }

public:

    string& getTitle() noexcept {
        return title;
    }

    [[nodiscard]] char getCartridgeType() const noexcept {
        return cartridgeType;
    }

    [[nodiscard]] char getRomSize() const noexcept {
        return romSize;
    }

    [[nodiscard]] char getRamSize() const noexcept {
        return ramSize;
    }

};

class Cartridge {

public:

    /**
     * Load a rom from the input stream.
     * @param stream
     */
    explicit Cartridge(ifstream& file): header(file) {

    }

protected:

    CartridgeHeader header;

    /* The content of the cartridge will be stored in this vector */
//    vector<unsigned char> rom;

};



#endif //GBE_CARTRIDGE_H
