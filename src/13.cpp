#include <algorithm>
#include <vector>

#include "util.h"

struct Expr {
  enum Type { NUMBER, LIST } type;
  int val{};
  std::vector<struct Expr> elements;

  auto operator<=>(const Expr &e) const {
    if (type == e.type) {
      if (type == Expr::NUMBER) return val <=> e.val;

      auto res = std::strong_ordering::equal;
      auto [els_min, els_max] = std::minmax({elements.size(), e.elements.size()});
      for (int i = 0; i < els_max; i++) {
        if (els_min != els_max) {
          if (i == elements.size())
            return std::strong_ordering::less;
          else if (i == e.elements.size())
            return std::strong_ordering::greater;
        }
        res = elements[i] <=> e.elements[i];
        if (res != std::strong_ordering::equal) break;
      }
      return res;
    } else if (type == Expr::NUMBER) {
      auto tmp = Expr{Expr::LIST, 0, {*this}};
      return tmp <=> e;
    } else {
      auto tmp = Expr{Expr::LIST, 0, {e}};
      return *this <=> tmp;
    }
  }
  bool operator==(const Expr &) const = default;
};

Expr parseDigit(std::istream &is) {
  int val;
  is >> val;
  return {Expr::NUMBER, val};
}

Expr parseList(std::istream &is) {
  Expr e{Expr::LIST};

  is.ignore();  // [.
  while (is.peek() != ']') {
    if (std::isdigit(is.peek()))
      e.elements.push_back(parseDigit(is));
    else if (is.peek() == '[')
      e.elements.push_back(parseList(is));
    if (is.peek() == ',') is.ignore();
  }
  is.ignore();  // ].

  return e;
}

Expr parseExpr(const std::string &s) {
  std::istringstream iss(s);
  return parseList(iss);
}

auto doPart1(const std::vector<Expr> &exprs) {
  int sum = 0;
  for (int i = 1; i < exprs.size(); i += 2) {
    if (exprs[i - 1] < exprs[i]) sum += (i + 1) / 2;
  }
  return sum;
}

auto doPart2(std::vector<Expr> exprs) {
  auto div1 = parseExpr("[[2]]");
  auto div2 = parseExpr("[[6]]");
  exprs.push_back(div1);
  exprs.push_back(div2);

  std::sort(exprs.begin(), exprs.end());
  auto it1 = std::find(exprs.begin(), exprs.end(), div1);
  auto it2 = std::find(exprs.begin(), exprs.end(), div2);
  return (it1 - exprs.begin() + 1) * (it2 - exprs.begin() + 1);
}

int main() {
  auto ifs = getInputStream("13.txt");
  std::vector<Expr> exprs;
  for (std::string line; ifs >> line;) exprs.push_back(parseExpr(line));

  outputAndVerify("Part 1", doPart1(exprs), 6070);
  outputAndVerify("Part 2", doPart2(exprs), 20758);

  return 0;
}
