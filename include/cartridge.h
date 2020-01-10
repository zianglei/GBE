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
#include <vector>
#include "address.h"

using uchar = unsigned char;

class CartridgeHeader {

public:
    explicit CartridgeHeader(std::istream & cartridge) {
        if (!cartridge) {
            throw std::invalid_argument("The cartridge is not opened");
        }

        char contents[CHECKSUM_SCALE] = {}; // only used to calculate checksum.

        // get the header checksum and verify the header.
        cartridge.seekg(CHECKSUM_OFFSET);
        if (!cartridge) {
            throw std::invalid_argument("Cannot get header checksum from the stream!");
        }
        uchar headersum;
        cartridge >> headersum;
        cartridge.seekg(CHECKSUM_BEGIN_OFFSET);
        cartridge.read(contents, CHECKSUM_SCALE);

        if (!isValid(contents, headersum)) {
            throw std::invalid_argument("The cartridge header checksum is wrong!");
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

    explicit CartridgeHeader(std::vector<uchar> const & bytes) {
        if (bytes.size() != 80) {
            throw std::invalid_argument("The vector length is not equal to the header length.");
        }

        char contents[CHECKSUM_SCALE];
        uchar checksum = bytes[CHECKSUM_OFFSET];

        copy(bytes.cbegin() + CHECKSUM_BEGIN_OFFSET, bytes.cbegin() + CHECKSUM_BEGIN_OFFSET + CHECKSUM_SCALE, contents);
        if (!isValid(contents, checksum)) {
            throw std::invalid_argument("The cartridge header checksum is wrong!");
        }

        auto it = bytes.cbegin();
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
    std::array<uchar, 4> entryPoint{};             // 0100h-0103h
    std::array<uchar, 48> logo{};                   // 0104h-0133h

    std::string title;
    std::string newLicenseCode;

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

    std::string& getTitle() noexcept {
        return title;
    }

    [[nodiscard]] std::array<uchar, 4> const & getEntryPoint() const noexcept {
        return entryPoint;
    }

    [[nodiscard]] std::array<uchar, 48> const & getLogo() const noexcept {
        return logo;
    }

    [[nodiscard]] std::string getLicenseCode() const noexcept {
        if (oldLicenseCode == 0x33) {
            return newLicenseCode;
        }
        return std::string{static_cast<char>(oldLicenseCode)};
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
    explicit Cartridge(std::ifstream& file){
        file.read(reinterpret_cast<char *>(_data.data()), file.tellg());
        _header = std::make_unique<CartridgeHeader>(_data);
        _capacity = _data.size();
    }

    explicit Cartridge(std::vector<uchar>&& cartridge) {
        _data = std::move(cartridge);
        _capacity = _data.size();
    }

public:

    virtual uchar read(const Address& address) const = 0;
    virtual void write(const Address& address, uchar data)  = 0;

protected:

    enum class CartridgeType {
        None,
        MBC1,
        MBC2,
        MBC3,
        MBC4,
        MBC5,
        HuC1
    };
    static CartridgeType getType(uchar type) {
        switch (type) {
            case 0x00:
                return CartridgeType::None;
            case 0x01:
            case 0x02:
            case 0x03:
                return CartridgeType::MBC1;
            case 0x05:
            case 0x06:
                return CartridgeType::MBC2;
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                return CartridgeType::MBC3;
            case 0x15:
            case 0x16:
            case 0x17:
                return CartridgeType::MBC4;
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
                return CartridgeType::MBC5;
            case 0xFF:
                return CartridgeType::HuC1;
            default:
                throw std::invalid_argument("Not implemented cartridge type");
        }
    }


    std::unique_ptr<CartridgeHeader> _header;
    std::vector<uchar> _data;
    size_t _capacity;


};

/**
 * 32KByte ROM only.
 */
class RomCartridge: public Cartridge {

public:

    [[nodiscard]] uchar read(const Address& address) const override;
    /**
     * The ROM is not writable.
     */
    void write(const Address &address, unsigned char data) override;
};

/**
 *
 */
class MBC1: public Cartridge {

public:
    explicit MBC1(std::ifstream &file) : Cartridge(file){}
    explicit MBC1(std::vector<uchar> &data) : Cartridge(std::move(data)) {}

    [[nodiscard]] uchar read(const Address &address) const override;
    void write(const Address &address, uchar data) override;

private:
    /**
     * Before external RAM can be read or written,
     * it must be enabled by writing to this address space
     */
    uchar ramEnabled{0x00};
    /* Writing to this address space selects the lower 5 bits of
     * the ROM Bank Number (in range 01-1Fh) */
    uchar romBankNumber{0x01};
    /**
     * This 2bit register can be used to select a RAM Bank in range from 00-03h,
     *  or to specify the upper two bits (Bit 5-6) of the ROM Bank number,
     *  depending on the current ROM/RAM Mode.
     */
    uchar ramBankNumber{0x00};
    /**
     * This register selects whether the two bits of the above register should be used as
     * upper two bits of the ROM bank, or as RAM Bank Number.
     */
    uchar modeSelect{0x00};

};

class MBC2: public Cartridge {

};



#endif //GBE_CARTRIDGE_H
