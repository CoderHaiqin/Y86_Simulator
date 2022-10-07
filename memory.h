#ifndef MEMORY_H
#define MEMORY_H

#include "constant.h"
#include <iostream>

class Memory
{
public:
    byte data[MEMORY_SIZE];

    Memory();
    bool get(int address, byte* dst);
    bool getWord(int address, word* dst);
    bool set(int address, const byte value);
    bool setWord(int address, const word dst);
};

#endif // MEMORY_H
