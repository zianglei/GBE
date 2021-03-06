/**
 * @file test-cartridge.cpp
 * @brief Test cartridge functions
 * @date 2019/12/22
 */

#include <catch.hpp>
#include <cartridge.h>
#include <vector>
#include <iostream>
#include <sstream>

/**
 * Test CartridgeHeader class can correctly parse cartridge header from vector.
 */
TEST_CASE("Cartridge header test", "[cartridge]") {

    std::vector<uchar> test_cartridge_data {
            0x00, 0xC3, 0x50, 0x01, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
            0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6,
            0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F,
            0xBB, 0xB9, 0x33, 0x3E, 0x5A, 0x45, 0x4C, 0x44, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x02, 0x01, 0x01, 0x02, 0x6A, 0x3A, 0xEE
    };

    std::array<uchar, 4> verify_entry_point {0x00, 0xC3, 0x50, 0x01};
    std::array<uchar, 48> verify_logo {
            0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D,
            0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
            0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08,
            0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
            0xDC, 0xCC, 0x6E, 0xE6,0xDD, 0xDD,
            0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63,
            0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC,
            0x99, 0x9F,0xBB, 0xB9, 0x33, 0x3E};

    SECTION("Get header from the vector") {

        CartridgeHeader header(test_cartridge_data);

        REQUIRE(header.getTitle() == "ZELDA");
        REQUIRE(header.getEntryPoint() == verify_entry_point);
        REQUIRE(header.getLogo() == verify_logo);
        REQUIRE(header.getLicenseCode() == std::string{0x01});
        REQUIRE(!header.isEnableSGB());
        REQUIRE(header.getCartridgeType() == 0x03);
        REQUIRE(header.getRomSize() == 0x04);
        REQUIRE(header.getRamSize() == 0x02);
        REQUIRE(header.getDestinationCode() == 0x01);
        REQUIRE(header.getVersion() == 0x02);
    }

    SECTION("Get header from the istream") {
        std::istringstream s(std::string(reinterpret_cast<const char *>(test_cartridge_data.data()), test_cartridge_data.size()));
        CartridgeHeader header(s);

        REQUIRE(header.getTitle() == "ZELDA");
        REQUIRE(header.getEntryPoint() == verify_entry_point);
        REQUIRE(header.getLogo() == verify_logo);
        REQUIRE(header.getLicenseCode() == std::string{0x01});
        REQUIRE(!header.isEnableSGB());
        REQUIRE(header.getCartridgeType() == 0x03);
        REQUIRE(header.getRomSize() == 0x04);
        REQUIRE(header.getRamSize() == 0x02);
        REQUIRE(header.getDestinationCode() == 0x01);
        REQUIRE(header.getVersion() == 0x02);
    }
}

TEST_CASE("MBC1 Test", "[cartridge]") {

    std::vector<uchar> romBank0(16384, 0);
    std::vector<uchar> romBank1(16384, 1);
    romBank0.insert(romBank0.end(),
            std::make_move_iterator(romBank1.begin()),
            std::make_move_iterator(romBank1.end()));

    MBC1 c{romBank0};

    SECTION("Normal read") {
        REQUIRE(c.read(0) == 0);
        REQUIRE(c.read(0x3FFF) == 0);
        REQUIRE(c.read(0x4000) == 1);
        REQUIRE(c.read(0x7FFF) == 1);
    }

    // TODO: Add write test.

}


