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
    explicit CartridgeHeader(ifstream & cartridge) {
        if (!cartridge) {
            throw invalid_argument("The cartridge is not opened");
        }

        char contents[CHECKSUM_SCALE] = {}; // only used to calculate checksum.

        // get the header checksum and verify the header.
        cartridge.seekg(CHECKSUM_OFFSET);
        if (!cartridge) {
            throw invalid_argument("Cannot get header checksum from the stream!");
        }
        uchar headersum;
        cartridge >> headersum;
        cartridge.seekg(CHECKSUM_BEGIN_OFFSET);
        cartridge.read(contents, CHECKSUM_SCALE);

        if (!isValid(contents, headersum)) {
            throw invalid_argument("The cartridge header checksum is wrong!");
        }

        cartridge.seekg(0);
        cartridge.read(reinterpret_cast<char*>(entryPoint.data()), entryPoint.size());
        cartridge.read(reinterpret_cast<char*>(logo.data()), logo.size());

        // fixme: Only support non-CGB mode for now.
        cartridge.width(16);
        cartridge >> title;
        cartridge.width(2);
        cartridge >> newLicenseCode;

        cartridge.width(1);
        cartridge >> sgbFlag >> cartridgeType >> romSize >> ramSize
                >> destinationCode >> oldLicenseCode >> maskRomVerNumber
                >> headerChecksum >> globalChecksum;

        // Delete null chars at the end of the title.
        for (auto sit = title.begin(); sit != title.end(); ++sit) {
            if (!isalnum(*sit)) {
                title.erase(sit, title.end());
                break;
            }
        }
    }

    explicit CartridgeHeader(vector<uchar> const & cartridge) {
        if (cartridge.size() != 80) {
            throw invalid_argument("The vector length is not equal to the header length.");
        }

        char contents[CHECKSUM_SCALE];
        uchar checksum = cartridge[CHECKSUM_OFFSET];

        copy(cartridge.cbegin() + CHECKSUM_BEGIN_OFFSET, cartridge.cbegin() + CHECKSUM_BEGIN_OFFSET + CHECKSUM_SCALE, contents);
        if (!isValid(contents, checksum)) {
            throw invalid_argument("The cartridge header checksum is wrong!");
        }

        auto it = cartridge.cbegin();
        copy(it, it + entryPoint.size(), entryPoint.data()); it += entryPoint.size();
        copy(it, it + logo.size(), logo.data()); it += logo.size();

        title.append(it, it + 16); it += 16;

        newLicenseCode.append(it, it + 2); it += 2;
        sgbFlag = *it++;
        cartridgeType = *it++;
        romSize = *it++;
        ramSize = *it++;
        destinationCode = *it++;
        oldLicenseCode = *it++;
        maskRomVerNumber = *it++;
        headerChecksum = *it++;
        globalChecksum = (((*it) << 8u) | (*(it + 1)));

        // Delete null chars at the end of the title.
        for (auto sit = title.begin(); sit != title.end(); ++sit) {
            if (!isalnum(*sit)) {
                title.erase(sit, title.end());
                break;
            }
        }
    }

private:
                                              // byte position
    array<uchar, 4> entryPoint{};             // 0100h-0103h
    array<uchar, 48> logo{};                   // 0104h-0133h

    string title = "";
    string newLicenseCode = "";

    uchar sgbFlag = 0;
    uchar cartridgeType = 0;
    uchar romSize = 0;
    uchar ramSize = 0;
    uchar destinationCode = 0;
    uchar oldLicenseCode = 0;
    uchar maskRomVerNumber = 0;
    uchar headerChecksum = 0;
    uint16_t globalChecksum = 0;              // 014Eh-014Fh

private:
    static constexpr int CHECKSUM_SCALE = 25;
    static constexpr int HEADER_LENGTH = 80;
    static constexpr int CHECKSUM_BEGIN_OFFSET = 52; // begin from 0x134;
    static constexpr int CHECKSUM_OFFSET = 77;  // 0x014D

private:
    /**
     * Calculate the 8-bit checksum:
     *   x=0:FOR i=0134h TO 014Ch: x=x-MEM[i]-1:NEXT
     * @return true if the calculated checksum is equal to the checksum in the header.
     */
    static bool isValid(char (&contents)[CHECKSUM_SCALE], uchar checksum) noexcept {
        uint8_t x = 0;
        for (uint8_t b: contents) {
            x -= b + 1;
        }
        return x == checksum;
    }

public:

    string& getTitle() noexcept {
        return title;
    }

    [[nodiscard]] array<uchar, 4> const & getEntryPoint() const noexcept {
        return entryPoint;
    }

    [[nodiscard]] array<uchar, 48> const & getLogo() const noexcept {
        return logo;
    }

    [[nodiscard]] string getLicenseCode() const noexcept {
        if (oldLicenseCode == 0x33) {
            return newLicenseCode;
        }
        return string{static_cast<char>(oldLicenseCode)};
    };

    [[nodiscard]] bool isEnableSGB() const noexcept {
        return sgbFlag == 0x03;
    }

    [[nodiscard]] uchar getCartridgeType() const noexcept {
        return cartridgeType;
    }

    [[nodiscard]] uchar getRomSize() const noexcept {
        return romSize;
    }

    [[nodiscard]] uchar getRamSize() const noexcept {
        return ramSize;
    }

    [[nodiscard]] uchar getDestinationCode() const noexcept {
        return destinationCode;
    }

    [[nodiscard]] uchar getVersion() const noexcept {
        return maskRomVerNumber;
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
