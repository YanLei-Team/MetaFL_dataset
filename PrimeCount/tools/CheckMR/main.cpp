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
  ll x;
};

struct O {
  ll y; // pi(x)
};

bool isPrime(ll x) {
  if (x < 2)
    return false;
  else if (x == 2)
    return true;
  for (int i = 2, up = floor(sqrt(x)); i <= up; i++)
    if (x % i == 0)
      return false;
  return true;
}

struct Case {
  I i;
  O o;
  Case() = default;
  Case(I i, O o) : i(i), o(o) {}
};

// ------------------------------- MR 1 -------------------------------
//  ProgramID = Pi
//       Name = Increasing
// Description = pi(i)<=pi(j) if i<=j
// ReferenceID =
bool MR1(Case &source, vector<Case> &follows) {
  assert(follows.size() == 1);
  return source.o.y <= follows[0].o.y;
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = Pi
//       Name = Counting
// Description = pi(i+1)=pi(i)+1 if i+1 is prime;
//				 pi(i+1)=pi(i) if i+1 is not prime
// ReferenceID =
bool MR2(Case &source, vector<Case> &follows) {
  assert(follows.size() == 1);
  Case &follow = follows[0];
  return source.o.y == follow.o.y - (isPrime(follow.i.x) ? 1 : 0);
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = Pi
//       Name = Second Hardy–Littlewood conjecture
// Description = pi(x)>=pi(x+y)-pi(y) for x,y>=2 (verified in 1e5)
// ReferenceID =
bool MR3(Case &source, vector<Case> &follows) {
  assert(follows.size() == 2);
  return source.o.y >= follows[0].o.y - follows[1].o.y;
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
  istringstream(line) >> word >> word >> ret.y;
  return ret;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%d\n", nMR);
    return 0;
  }
  assert(argc >= 7);
  string pathInputs(argv[1]);
  int iMR = atoi(argv[2]);
  string scheme, id;
  I input;
  istringstream(argv[3]) >> scheme >> id >> input.x;
  Case source(input, readO(argv[4]));
  vector<Case> follows;
  int nFollow = (argc - 5) / 2;
  for (int i = 0; i < nFollow; i++) {
    istringstream(argv[5 + i * 2]) >> scheme >> id >> input.x;
    follows.push_back(Case(input, readO(argv[5 + i * 2 + 1])));
  }
  return MR(iMR, source, follows) ? 0 : 1;
}
