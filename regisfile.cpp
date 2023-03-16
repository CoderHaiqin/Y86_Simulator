#include "regisfile.h"

RegisFile::RegisFile() {
  for (int i = 0; i < 16; i++) {
    data[i] = 0;
  }
}

word RegisFile::get(byte src) {
  if (src >= 16) {
    std::cout << "invalid register";
    return 0;
  }
  return data[src];
}

void RegisFile::set(byte dst, word value) {
  if (dst >= 16) {
    std::cout << "invalid register";
    return;
  }
  data[dst] = value;
}
