#include <algorithm>

#include "util.h"

int getIdx(char c) { return std::islower(c) ? c - 'a' : c - 'A' + 26; }

int doPart1(std::ifstream is) {
  int total{};

  for (std::string s; is >> s;) {
    uint8_t buf[26 * 2]{};
    for (auto i = 0u; i < s.size(); i++) {
      if (i < s.size() / 2) {
        buf[getIdx(s[i])] = 1;
      } else if (buf[getIdx(s[i])]) {
        total += getIdx(s[i]) + 1;
        break;
      }
    }
  }
  return total;
}

int doPart2(std::ifstream is) {
  uint8_t buf[26 * 2]{};
  int total{};

  for (std::string s; is >> s;) {
    std::ranges::sort(s);
    s.erase(std::unique(s.begin(), s.end()), s.end());
    for (auto c : s) {
      if (++buf[getIdx(c)] == 3) {
        total += getIdx(c) + 1;
        std::ranges::fill(buf, 0);
        break;
      }
    }
  }
  return total;
}

int main() {
  outputAndVerify("Part 1", doPart1(getInputStream("3.txt")), 8153);
  outputAndVerify("Part 2", doPart2(getInputStream("3.txt")), 2342);

  return 0;
}
