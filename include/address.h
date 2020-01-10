/**
 * @file address.h
 * @brief
 * @date 2020/1/10
 */

#ifndef GBE_ADDRESS_H
#define GBE_ADDRESS_H

class Address {

public:
    Address(uint16_t addr): location(addr) {}

public:
    [[nodiscard]] bool in(Address const & start, Address const & end) const {
        return (start.value() <= location) && (location <= end.value());
    }

    [[nodiscard]] bool in (uint16_t start, uint16_t end) const {
        return (start <= location) && (location <= end);
    }

    [[nodiscard]] uint16_t value() const {
        return location;
    }

    Address& operator+(const Address & rhs) {
        location += rhs.value();
        return *this;
    }

    Address& operator+(uint16_t rhs) {
        location += rhs;
        return *this;
    }

    Address& operator-=(uint16_t rhs) {
        if (location <= rhs) {
            location = 0;
        }
        location -= rhs;
        return *this;
    }

    Address operator-(uint16_t rhs) const {
        return Address{static_cast<uint16_t>(location - rhs < 0? 0: location - rhs)};
    }

    bool operator==(Address const & rhs) const {
        return location == rhs.value();
    }

    bool operator==(uint16_t rhs) const {
        return location == rhs;
    }

    bool operator <=(uint16_t rhs) const {
        return location <= rhs;
    }

    bool operator >=(uint16_t rhs) const {
        return !(*this <= rhs);
    }

    bool operator <(uint16_t rhs) const {
        return location < rhs;
    }

    bool operator >(uint16_t rhs) const {
        return !(*this < rhs);
    }


private:
    uint16_t location;
};

inline
std::string operator+(const char* lhs, const Address& rhs) {
    std::string addrStr;
    sprintf(addrStr.data(), "0x%X", rhs.value());
    return lhs + addrStr;
}

inline
std::string operator+(const Address& lhs, const char* rhs)  {
    std::string addrStr;
    sprintf(addrStr.data(), "0x%X", lhs.value());
    return addrStr + rhs;
}

#endif //GBE_ADDRESS_H
