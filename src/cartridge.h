/**
 * @file cartridge.h
 * @brief The cartridge class definition
 * @date 2019/12/18
 */

#ifndef GBE_CARTRIDGE_H
#define GBE_CARTRIDGE_H

#include <string>
#include <vector>

using namespace std;

class Cartridge {

public:
    /**
     * Load a cartridge from a rom file
     * @param rom_path: The rom file path
     * @throw invalid_argument: The file is not exists or not a valid rom file.
     */
    void load(string rom_path);

private:

    /* The content of the cartridge will be stored in this vector */
    vector<unsigned char> rom;

};



#endif //GBE_CARTRIDGE_H
