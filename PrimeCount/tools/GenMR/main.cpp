#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>

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
  for (int i = 2, up = sqrt(x); i <= up; i++)
    if (x % i == 0)
      return false;
  return true;
}

// ------------------------------- MR 1 -------------------------------
//  ProgramID = Pi
//       Name = Increasing
// Description = pi(i)<=pi(j) if i<=j
// ReferenceID =
vector<I> MR1(I sourceI, O /*sourceO*/) {
  I follow = sourceI;
  follow.x += random(100);
  return {follow};
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = Pi
//       Name = Counting
// Description = pi(i+1)=pi(i)+1 if i+1 is prime;
//				 pi(i+1)=pi(i) if i+1 is not prime
// ReferenceID =
vector<I> MR2(I sourceI, O /*sourceO*/) {
  I follow = sourceI;
  follow.x++;
  return {follow};
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = Pi
//       Name = Second Hardy–Littlewood conjecture
// Description = pi(x)>=pi(x+y)-pi(y) for x,y>=2 (verified in 1e5)
// ReferenceID =
vector<I> MR3(I sourceI, O /*sourceO*/) {
  if (sourceI.x < 2)
    return {};
  I followXY, followY;
  followY.x = max(2LL, random(sourceI.x) + 1);
  followXY.x = sourceI.x + followY.x;
  return {followXY, followY};
}

vector<vector<I> (*)(I, O)> MRs{MR1, MR2, MR3};
int nMR = MRs.size();

vector<I> MR(int iMR, I input, O output) {
  assert(1 <= iMR && iMR <= nMR);
  return MRs[iMR - 1](input, output);
}

O readO(string filename) {
  O ret;
  ifstream f(filename);
  string line, word;
  getline(f, line);
  istringstream(line) >> word >> word >> ret.y;
  return ret;
}

void testPool() {
  ifstream pool("./inputs/t");
  assert(pool.is_open());
  int i = 1;
  int limit = 10;
  string input, word, scheme, id;
  I sourceI;
  O sourceO;
  while (getline(pool, input)) {
    cout << "Test " << i << '\t' << string(30, '*') << '\n';
    cout << "\tSource:" << endl;
    cout << "\t\t" << input << '\n';
    //        cout<<"\tOutput:\t"<<output.toStdString()<<'\n';
    istringstream(input) >> scheme >> id >> sourceI.x;
    //    sourceO = readO("./outputs/t" + id);
    for (int iMR = 1; iMR <= nMR; iMR++) {
      vector<I> follows = MR(iMR, sourceI, sourceO);
      cout << "\tMR" << iMR << ":" << endl;
      for (I &follow : follows)
        cout << "\t\t" << scheme << ' ' << id << ' ' << follow.x << endl;
    }
    limit--;
    if (limit == 0)
      break;
    i++;
  }
  pool.close();
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("%d\n", nMR);
    return 0;
  }
  assert(argc == 5);
  string pathInputs(argv[1]);
  int iMR = atoi(argv[2]);
  string scheme, id;
  I input;
  istringstream(argv[3]) >> scheme >> id >> input.x;
  O output = readO(argv[4]);
  vector<I> follows = MR(iMR, input, output);
  if (!follows.empty()) {
    for (I &follow : follows)
      cout << "f " << id << ' ' << follow.x << endl;
  }

  //  testPool();

  return 0;
}
