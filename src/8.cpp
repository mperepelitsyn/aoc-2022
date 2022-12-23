#include <fstream>
#include <iostream>
#include <vector>

#include "util.h"

struct Point {
  int x;
  int y;
};

using Input = std::vector<std::vector<int>>;

Input parseInput(std::istream &is) {
  Input grid;
  for (std::string line; is >> line;) {
    std::vector<int> row;
    for (auto c : line) row.push_back(c - '0');
    grid.push_back(std::move(row));
  }
  return grid;
}

int getScenicScore(const Input &input, int xorig, int yorig) {
  int score = 1;
  for (auto &dir : {Point{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
    auto cnt = 0;
    for (auto x = xorig + dir.x, y = yorig + dir.y; x >= 0 && x < input.front().size() && y >= 0 && y < input.size();
         x += dir.x, y += dir.y) {
      cnt++;
      if (input[y][x] >= input[yorig][xorig]) break;
    }
    score *= cnt;
  }
  return score;
}

bool isVisibleFromOutside(const Input &input, int xorig, int yorig) {
  if (xorig == 0 || xorig == input.front().size() - 1 || yorig == 0 || yorig == input.size() - 1) return true;

  for (auto &dir : {Point{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
    for (auto x = xorig + dir.x, y = yorig + dir.y; x >= 0 && x < input.front().size() && y >= 0 && y < input.size();
         x += dir.x, y += dir.y) {
      if (input[y][x] >= input[yorig][xorig]) goto newdir;
    }
    return true;
  newdir:;
  }
  return false;
}

auto doPart1(const Input &input) {
  int visible{};
  for (int y = 0; y < input.size(); y++) {
    for (int x = 0; x < input.front().size(); x++) {
      visible += isVisibleFromOutside(input, x, y);
    }
  }
  return visible;
}

auto doPart2(const Input &input) {
  int max = INT32_MIN;
  for (int y = 0; y < input.size(); y++) {
    for (int x = 0; x < input.front().size(); x++) {
      max = std::max(max, getScenicScore(input, x, y));
    }
  }
  return max;
}

int main() {
  auto fin = getInputStream("8.txt");
  auto input = parseInput(fin);

  outputAndVerify("Part 1", doPart1(input), 1805);
  outputAndVerify("Part 2", doPart2(input), 444528);

  return 0;
}
