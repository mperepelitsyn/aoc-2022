#include <iostream>

#include "util.h"

int getScoreP1(char c1, char c2) {
  auto myhand = c2 - 'X';
  auto otherhand = c1 - 'A';
  auto diff = 1 - (otherhand + 3 + 1 - myhand) % 3;
  return myhand + 1 + 3 * diff + 3;
}

int getScoreP2(char c1, char c2) {
  auto otherhand = c1 - 'A';
  auto diff = c2 - 'X' - 1;
  auto myhand = (otherhand + 3 + diff) % 3;
  return myhand + 1 + 3 * diff + 3;
}

int main() {
  auto ifs = getInputStream("2.txt");
  int score1 = 0;
  int score2 = 0;

  for (char c1, c2; ifs >> c1 >> c2;) {
    score1 += getScoreP1(c1, c2);
    score2 += getScoreP2(c1, c2);
  }

  outputAndVerify("Part 1", score1, 8890);
  outputAndVerify("Part 2", score2, 10238);

  return 0;
}