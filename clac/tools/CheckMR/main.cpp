#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define random(x) (rd() % x)
random_device rd;

typedef long long ll;

struct I {
  vector<string> e;
};

template <typename T> istream &operator>>(istream &&in, vector<T> &v) {
  T t;
  while (in >> t)
    v.push_back(t);
  return in;
}

template <typename T> ostream &operator<<(ostream &out, const vector<T> &v) {
  for (unsigned i = 0; i < v.size(); i++) {
    if (i > 0)
      out << ' ';
    out << v[i];
  }
  return out;
}

struct O {
  double r{0.0};
};

struct Case {
  I i;
  O o;
  Case() = default;
  Case(I i, O o) : i(i), o(o) {}
};

double epsilon = 1e-12;
bool equal(double x, double y) {
//    printf("abs(%.12f - %.12f) < %.12f?\n",x,y,epsilon);
    return abs(x - y) < epsilon;
}

// ------------------------------- MR 1 -------------------------------
//  ProgramID = clac
//       Name = Subtract self
// Description = clac(x x -)=0
// ReferenceID =
bool MR1(Case & /*source*/, vector<Case> &follows) {
  assert(follows.size() == 1);
  return equal(follows[0].o.r, 0);
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = clac
//       Name = Sine
// Description = clac(x sin)=clac(pi x - sin)
// ReferenceID =
bool MR2(Case & /*source*/, vector<Case> &follows) {
  assert(follows.size() == 2);
  return equal(follows[0].o.r, follows[1].o.r);
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = clac
//       Name = Double
// Description = clac(x dup +)=clac(x 2 *)
// ReferenceID =
bool MR3(Case & /*source*/, vector<Case> &follows) {
  assert(follows.size() == 2);
  return equal(follows[0].o.r, follows[1].o.r);
}

vector<bool (*)(Case &, vector<Case> &)> MRs{MR1, MR2, MR3};
int nMR = MRs.size();

bool MR(int iMR, Case &source, vector<Case> &follows) {
  assert(1 <= iMR && iMR <= nMR);
  return MRs[iMR - 1](source, follows);
}

O readO(string filename) {
  O ret;
  ifstream f(filename);
  string line, word;
  getline(f, line);
  istringstream(line) >> ret.r;
  return ret;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%d\n", nMR);
    return 0;
  }
  assert(argc >= 6);
  string pathInputs(argv[1]);
  int iMR = atoi(argv[2]);
  string scheme, id;
  I input;
  istringstream(argv[3]) >> scheme >> id >> input.e;
  Case source(input, readO(argv[4]));
  vector<Case> follows;
  int nFollow = (argc - 4) / 2;
  for (int i = 0; i < nFollow; i++) {
    input.e.clear();
    istringstream(argv[5 + i * 2]) >> scheme >> id >> input.e;
    follows.push_back(Case(input, readO(argv[5 + i * 2 + 1])));
  }
  return MR(iMR, source, follows) ? 0 : 1;
}
