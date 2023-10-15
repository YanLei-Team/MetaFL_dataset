#include <cassert>
#include <iostream>

int main(int argc, char *argv[]) {
  int x{atoi(argv[1])}, y{atoi(argv[2])}, z{atoi(argv[3])}, m{z};
  if (y < z) {
    if (x < y)
      m = y;
    else if (x < z)
      m = x;
  } else {
    if (x > y)
      m = y;
    else if (x > z)
      m = x;
  }
  std::cout << m << '\n';
}
