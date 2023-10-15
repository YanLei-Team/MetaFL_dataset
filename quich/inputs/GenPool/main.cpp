#include <chrono>
#include <cstdio>
#include <functional>
#include <iostream>
#include <random>

using namespace std;
typedef long long ll;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);

uniform_int_distribution<ll> distributionInt;
int randInt(ll l, ll r) {
  return uniform_int_distribution<ll>(l, r)(generator);
}

normal_distribution<double> distributionReal;
double randReal(double mu, double std) {
  return normal_distribution<double>(mu, std)(generator);
}

string randOperand() { return to_string(randReal(5.0, 5.0)); }

vector<string> unaryOperators = {"log",   "sin",   "cos", "tan", "ceil",
                                 "floor", "round", "abs", "sqrt"};
string randUnaryOperator() {
  return unaryOperators[randInt(0, unaryOperators.size() - 1)];
}

vector<string> binaryOperators = {"+", "-", "*", "/"};
string randBinaryOperator() {
  return binaryOperators[randInt(0, binaryOperators.size() - 1)];
}

struct Expression {
  enum ExpressionType { Empty, Value, Unary, Binary };
  ExpressionType type;
  string val;
  string op;
  vector<Expression> exp;
  Expression() { type = Empty; }
  Expression(string val) : val(val) { type = Value; }
  Expression(string op, Expression exp) : op(op), exp({exp}) { type = Unary; }
  Expression(string op, Expression exp1, Expression exp2)
      : op(op), exp({exp1, exp2}) {
    type = Binary;
  }
};

Expression randExpresion(int depth) {
  if (depth == 0)
    return Expression();
  if (depth == 1 && randInt(0, 1))
    return Expression(randOperand());
  int dL = randInt(0, depth), dR = depth - dL;
  Expression exp;
  Expression expL = randExpresion(dL);
  Expression expR = randExpresion(dR);
  if (dL == 0)
    return Expression(randUnaryOperator(), expR);
  if (dR == 0)
    return Expression(randUnaryOperator(), expL);
  return Expression(randBinaryOperator(), expL, expR);
}

string exp2str(Expression e) {
  switch (e.type) {
  case e.Empty:
    return "";
  case e.Value:
    return e.val;
  case e.Unary:
    return e.op + "(" + exp2str(e.exp[0]) + ")";
  case e.Binary:
    return "(" + exp2str(e.exp[0]) + ")" + e.op + "(" + exp2str(e.exp[1]) + ")";
  default:
    return "";
  }
}

void printExpression(Expression e) { cout << exp2str(e) << endl; }

int main() {
  int N = 1e4;
  int minD = 1, maxD = 12;
  while (N--)
    printExpression(randExpresion(randInt(minD, maxD)));
  return 0;
}
