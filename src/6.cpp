#include <bitset>

#include "util.h"

int getStartOfMsgMarker(const std::string &input, int len) {
  std::bitset<32> w;
  for (auto i = 0; i < input.size(); i++) {
    w.flip(input[i] - 'a');
    if (i > len - 1) w.flip(input[i - len] - 'a');
    if (w.count() == len) return i + 1;
  }

  return -1;
}

int main() {
  auto fin = getInputStream("6.txt");
  std::string input;
  fin >> input;

  outputAndVerify("Part 1", getStartOfMsgMarker(input, 4), 1235);
  outputAndVerify("Part 2", getStartOfMsgMarker(input, 14), 3051);

  return 0;
}
