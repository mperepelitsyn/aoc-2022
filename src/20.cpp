#include <algorithm>
#include <vector>

#include "util.h"

using Input = std::vector<std::pair<long, int>>;

auto parseInput(std::istream &is) {
  Input buf;
  for (int d, i = 0; is >> d; i++) buf.emplace_back(d, i);
  return buf;
}

auto &wrapIdx(Input &buf, int i) {
  i = i % static_cast<int>(buf.size());
  if (i < 0)
    return buf[buf.size() + i];
  else
    return buf[i];
}

long mix(Input buf, long mul, size_t times) {
  std::ranges::for_each(buf, [mul](auto &p) { p.first *= mul; });

  while (times--) {
    for (int i = 0; i < buf.size(); i++) {
      auto it = std::ranges::find(buf, i, &std::pair<long, int>::second);
      int cur_pos = it - buf.begin();
      int new_pos = cur_pos + it->first % (static_cast<int>(buf.size()) - 1);

      if (cur_pos < new_pos) {
        for (int j = cur_pos; j < new_pos; j++) std::swap(wrapIdx(buf, j), wrapIdx(buf, j + 1));
      } else {
        for (int j = cur_pos; j > new_pos; j--) std::swap(wrapIdx(buf, j), wrapIdx(buf, j - 1));
      }
    }
  }

  auto it = std::ranges::find(buf, 0, &std::pair<long, int>::first);
  auto pos = it - buf.begin();
  long sum{};
  for (auto offset : {1000, 2000, 3000}) sum += buf[(pos + offset) % buf.size()].first;

  return sum;
}

int main() {
  auto ifs = getInputStream("20.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1", mix(input, 1, 1), 7228);
  outputAndVerify("Part 2", mix(input, 811589153, 10), 4526232706281);

  return 0;
}
