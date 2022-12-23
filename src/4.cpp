#include <regex>

#include "util.h"

int main() {
  auto fin = getInputStream("4.txt");
  std::regex reg(R"((\d+)-(\d+),(\d+)-(\d+))");
  int subsets{};
  int intersections{};

  for (std::string s; std::getline(fin, s);) {
    std::smatch m;
    std::regex_match(s, m, reg);
    auto min1 = std::stoi(m[1].str());
    auto max1 = std::stoi(m[2].str());
    auto min2 = std::stoi(m[3].str());
    auto max2 = std::stoi(m[4].str());

    if ((min1 >= min2 && max1 <= max2) || (min2 >= min1 && max2 <= max1)) subsets++;
    if ((max1 >= min2 && min1 <= max2) || (max2 >= min1 && min2 <= max1)) intersections++;
  }

  outputAndVerify("Part 1", subsets, 556);
  outputAndVerify("Part 2", intersections, 876);

  return 0;
}
