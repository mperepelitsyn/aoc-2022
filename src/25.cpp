#include <algorithm>
#include <vector>

#include "util.h"

using Input = std::vector<std::string>;

auto parseInput(std::istream &is) {
  Input ret;
  for (std::string line; is >> line;) {
    ret.push_back(line);
  }
  return ret;
}

int to_digit(char c) {
  if (c == '=') return -2;
  if (c == '-') return -1;
  return c - '0';
}

char to_char(int d) {
  if (d == -2) return '=';
  if (d == -1) return '-';
  return '0' + d;
}

auto sum(const std::string &a, const std::string &b) {
  std::string ret;

  int i = a.size() - 1, j = b.size() - 1;
  int carry = 0;
  while (carry || i >= 0 || j >= 0) {
    auto d1 = i >= 0 ? to_digit(a[i]) : 0;
    auto d2 = j >= 0 ? to_digit(b[j]) : 0;

    auto sum = d1 + d2 + carry;
    if (sum > 2) {
      sum -= 5;
      carry = 1;
    } else if (sum < -2) {
      sum += 5;
      carry = -1;
    } else {
      carry = 0;
    }

    ret.push_back(to_char(sum));
    --i, --j;
  }
  std::ranges::reverse(ret);
  return ret;
}

auto doPart1(const Input &input) {
  std::string ret;
  for (auto &num : input) {
    ret = sum(ret, num);
  }
  return ret;
}

int main() {
  auto ifs = getInputStream("25.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1", doPart1(input), "2=112--220-=-00=-=20");

  return 0;
}
