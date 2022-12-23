#include <vector>

#include "util.h"

auto parseCrates(std::ifstream &fs) {
  std::vector<std::string> lines;
  std::vector<std::string> stacks;

  for (std::string line; std::getline(fs, line);) {
    if (line.empty()) break;
    lines.push_back(std::move(line));
  }

  for (auto i = 0u; i < lines.back().size(); i++) {
    if (!std::isdigit(lines.back()[i])) continue;

    std::string stack;
    for (int j = lines.size() - 2; j >= 0; j--) {
      if (!isalpha(lines[j][i])) break;
      stack.push_back(lines[j][i]);
    }
    stacks.push_back(std::move(stack));
  }

  return stacks;
}

void moveCratesP1(std::vector<std::string> &stacks, int num, int from, int to) {
  for (auto i = 0u; i < num; i++) {
    stacks[to - 1].push_back(stacks[from - 1].back());
    stacks[from - 1].pop_back();
  }
}

void moveCratesP2(std::vector<std::string> &stacks, int num, int from, int to) {
  auto &stack_from = stacks[from - 1];
  auto &stack_to = stacks[to - 1];
  stack_to.append(stack_from.substr(stack_from.size() - num));
  stack_from.erase(stack_from.size() - num);
}

auto getStacksTop(const std::vector<std::string> &stacks) {
  std::string out;
  for (auto &s : stacks) out.push_back(s.back());
  return out;
}

int main() {
  auto fin = getInputStream("5.txt");
  auto stacks1 = parseCrates(fin);
  auto stacks2 = stacks1;

  int num, from, to;
  std::string garbage;
  while (fin >> garbage >> num >> garbage >> from >> garbage >> to) {
    moveCratesP1(stacks1, num, from, to);
    moveCratesP2(stacks2, num, from, to);
  }

  outputAndVerify("Part 1", getStacksTop(stacks1), "FWNSHLDNZ");
  outputAndVerify("Part 2", getStacksTop(stacks2), "RNRGDNFQG");

  return 0;
}
