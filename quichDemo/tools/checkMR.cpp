#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define random(x) (rd() % x)
random_device rd;

typedef long long ll;

struct I {
  string e;
};

struct O {
  double r;
};

struct Case {
  I i;
  O o;
  Case() = default;
  Case(I i, O o) : i(i), o(o) {}
};

double epsilon = 1e-9;
bool equal(double x, double y) {
//    printf("abs(%.9f - %.9f) < %.9f?\n",x,y,epsilon);
    return abs(x - y) < epsilon;
}

// ------------------------------- MR 1 -------------------------------
//  ProgramID = quich
//       Name = Subtract self
// Description = quich( (x)-(x) )=0
// ReferenceID =
bool MR1(Case & /*source*/, vector<Case> &follows) {
  assert(follows.size() == 1);
  return equal(follows[0].o.r, 0);
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = quich
//       Name = Sine
// Description = quich( sin(x) )=quich( sin(pi-x) )
// ReferenceID =
bool MR2(Case & /*source*/, vector<Case> &follows) {
  assert(follows.size() == 2);
  return equal(follows[0].o.r, follows[1].o.r);
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = quich
//       Name = Double
// Description = quich( x+x )=quich( (x)*2 )
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
