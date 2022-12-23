#include <algorithm>
#include <numeric>
#include <vector>

#include "util.h"

int main() {
  auto in = getInputStream("1.txt");
  int cal{};
  std::vector<int> cals;

  for (std::string s; std::getline(in, s);) {
    if (s.empty()) {
      cals.push_back(cal);
      cal = 0;
      continue;
    }
    cal += stoi(s);
  }
  cals.push_back(cal);

  std::sort(cals.begin(), cals.end(), std::greater<>());

  outputAndVerify("Part 1", cals.front(), 71471);
  outputAndVerify("Part 2", std::reduce(cals.begin(), cals.begin() + 3), 211189);

  return 0;
}
