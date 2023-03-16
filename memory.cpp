#include "memory.h"

Memory::Memory() {
  // 初始化，设为0
  for (int i = 0; i < MEMORY_SIZE; i++) {
    data[i] = 0;
  }
}

bool Memory::get(int address, byte* dst) {
  if (address < 0 || address >= MEMORY_SIZE) {
    std::cout << "invalid address";
    return false;
  } else {
    *dst = data[address];
    return true;
  }
}

bool Memory::getWord(int address, word* dst) {
  if (address < 0 || address >= MEMORY_SIZE - 8) {
    std::cout << "invalid address";
    return false;
  }

  word tmp = 0;
  *dst = 0;
  for (int i = 0; i < 8; i++) {
    tmp = data[address + i];
    *dst |= (tmp << (8 * i));
  }
  return true;
}

bool Memory::set(int address, const byte value) {
  if (address < 0 || address >= MEMORY_SIZE) {
    std::cout << "invalid address";
    return false;
  } else {
    data[address] = value;
    return true;
  }
}

bool Memory::setWord(int address, const word dst) {
  if (address < 0 || address >= MEMORY_SIZE) {
    std::cout << "invalid address";
    return false;
  } else {
    byte tmp = 0;
    for (int i = 0; i < 8; i++) {
      tmp = (dst & (0xff << (8 * i))) >> (8 * i);
      data[address + i] = tmp;
    }
    return true;
  }
}
