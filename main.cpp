#include "simulator.h"
using namespace std;

int main(int argc, char *argv[]) {
  Simulator s;
  s.load("D:\\test.txt");
  std::string str;
  while (1) {
    if (s.W.stat != AOK) {
      std::cout << "end" << std::endl;
      std::getline(cin, str);
    }
    s.tick();
  }
}
