#ifndef REGISFILE_H
#define REGISFILE_H
#include <iostream>

#include "constant.h"

class RegisFile {
 public:
  word data[16];
  word get(byte src);
  void set(byte dst, word value);

  RegisFile();
};

#endif  // REGISFILE_H
