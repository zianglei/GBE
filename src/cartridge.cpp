/**
 * @file cartridge.cpp
 * @brief
 * @date 2019/12/18
 */

#include <cartridge.h>
#include <stdexcept>


unsigned char
RomCartridge::read(const Address &address) const {
    if (address > 0x7FFF) {
        throw std::invalid_argument("Invalid address " + address + "to read!");
    }
    return _data.at(address.value());
}

void
RomCartridge::write(const Address & address, unsigned char data) {
    throw std::invalid_argument("Cannot write to ROM");
}

uchar MBC1::read(const Address &address) const {
    if (address >= _capacity)
        throw std::invalid_argument("The address " + address + " is out of scale.");

    if (address.in(0, 0x3FFF)) {
        // ROM Band 00
        return _data.at(address.value());
    } else if (address.in(0x4000, 0x7FFF)) {
        // Read from the extended ROM;
        uint16_t romBankIndex = modeSelect == 0x00 ?
                (ramBankNumber & 0xE0u) || (romBankNumber & 0x1Fu) : romBankNumber & 0x1Fu;
        Address bankAddr = romBankIndex * 0x4000;
        Address bankAddrOffset = address - 0x4000;
        return _data.at((bankAddr + bankAddrOffset).value());

    } else if (address.in(0xA000, 0xBFFF)) {
        // Read from external RAM Bank 00-03
        if ((ramEnabled & 0x0Fu) == 0x0A) {
            if (modeSelect == 0x00) {
                // only RAM Bank 00h can be used.
                return _data.at(address.value());
            } else {
                Address ramBankAddr = ramBankNumber * 0x2000;
                Address ramBankAddrOffset = address - 0xA000;
                return _data.at((ramBankAddr + ramBankAddrOffset).value());
            }
        } else {
            throw std::logic_error("The RAM bank is not enabled!");
        }
    } else {
        throw std::invalid_argument("Invalid address " + address + " to read.");
    }
}

void MBC1::write(const Address & address, uchar data) {
    if (address >= _capacity)
        throw std::invalid_argument("The address " + address + " is out of scale.");

    if (address.in(0, 0x1FFF)) {
        // Write to the RAM Enable register
        ramEnabled = data;
    } else if (address.in(0x2000, 0x3FFF)) {
        // Write to the ROM Bank Number register
        if (data == 0x00) data = 0x01;
        romBankNumber = data;
    } else if (address.in(0x4000, 0x5FFF)) {
        // Write to the RAM Bank Number / Upper Bits of ROM Bank Number
        ramBankNumber = data;
    } else if (address.in(0x6000, 0x7FFF)) {
        // Write to ROM/RAM Select register
        modeSelect = data;
    } else if (address.in(0xA000, 0xBFFF)) {
        // Write to RAM Bank 00-03
        if ((ramEnabled & 0x0Fu) == 0x0A) {
            if (modeSelect == 0x00) {
                // ROM Banking Mode
                _data.at(address.value()) = data;
            } else {
                Address ramBankAddr = ramBankNumber * 0x2000; // 8KByte each bank
                Address ramBankAddrOffset = address - 0xA000;
                _data.at((ramBankAddrOffset + ramBankAddr).value()) = data;
            }
        } else throw std::logic_error("The RAM bank is not enabled!");
    } else {
        throw std::invalid_argument("Invalid address " + address + " to write.");
    }
}







