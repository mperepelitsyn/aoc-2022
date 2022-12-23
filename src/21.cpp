#include <unordered_map>

#include "util.h"

struct Expr {
  enum Type { NUM, BINOP } type;
  int64_t val{};
  std::string op1;
  std::string op2;
  char op{};
  bool needs_solving{false};
};

using Input = std::unordered_map<std::string, Expr>;

auto parseInput(std::istream &is) {
  Input input;
  std::string var, op1, op2;
  char op;
  while (is >> var >> op1) {
    if (std::isdigit(op1.front())) {
      input.emplace(var.substr(0, var.size() - 1), Expr{Expr::NUM, std::stoi(op1)});
    } else {
      is >> op >> op2;
      input.emplace(var.substr(0, var.size() - 1), Expr{Expr::BINOP, 0, std::move(op1), std::move(op2), op});
    }
  }
  return input;
}

auto eval(const Input &in, const std::string &var) {
  auto &e = in.at(var);
  if (e.type == Expr::NUM) {
    return e.val;
  } else {
    auto op1 = eval(in, e.op1);
    auto op2 = eval(in, e.op2);
    switch (e.op) {
      case '+':
        return op1 + op2;
      case '-':
        return op1 - op2;
      case '*':
        return op1 * op2;
      case '/':
        return op1 / op2;
    }
  }
  abort();
}

bool markXExprTree(Input &in, const std::string &var, const std::string &x_var) {
  auto &e = in.at(var);
  if (var == x_var) {
    e.needs_solving = true;
  } else if (e.type == Expr::NUM) {
    e.needs_solving = false;
  } else {
    e.needs_solving = markXExprTree(in, e.op1, x_var) || markXExprTree(in, e.op2, x_var);
  }
  return e.needs_solving;
}

long solve(const Input &in, long result, const std::string &var, const std::string &x_var) {
  if (var == x_var) return result;

  auto &e = in.at(var);

  long operand;
  std::string solve_var;
  if (in.at(e.op1).needs_solving) {
    operand = eval(in, e.op2);
    solve_var = e.op1;
  } else {
    operand = eval(in, e.op1);
    solve_var = e.op2;
  }

  long operand_x{};
  switch (e.op) {
    case '+':
      operand_x = result - operand;
      break;
    case '-':
      operand_x = e.op1 == solve_var ? result + operand : operand - result;
      break;
    case '*':
      operand_x = result / operand;
      break;
    case '/':
      operand_x = e.op1 == solve_var ? result * operand : operand / result;
  }

  return solve(in, operand_x, solve_var, x_var);
}

auto solve(Input &in, const std::string &root_var, const std::string &x_var) {
  markXExprTree(in, root_var, x_var);

  auto root = in.at(root_var);
  if (in.at(root.op1).needs_solving) {
    return solve(in, eval(in, root.op2), root.op1, x_var);
  } else {
    return solve(in, eval(in, root.op1), root.op2, x_var);
  }
}

int main() {
  auto ifs = getInputStream("21.txt");
  auto input = parseInput(ifs);

  outputAndVerify("Part 1", eval(input, "root"), 353837700405464);
  outputAndVerify("Part 2", solve(input, "root", "humn"), 3678125408017);

  return 0;
}
