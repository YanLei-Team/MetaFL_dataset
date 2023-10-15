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
  string e;
};

struct O {
  double r;
};

// ------------------------------- MR 1 -------------------------------
//  ProgramID = quich
//       Name = Subtract self
// Description = quich( (x)-(x) )=0
// ReferenceID =
vector<I> MR1(I sourceI, O /*sourceO*/) {
  return {{"(" + sourceI.e + ")-(" + sourceI.e + ")"}};
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = quich
//       Name = Sine
// Description = quich( sin(x) )=quich( sin(pi-x) )
// ReferenceID =
vector<I> MR2(I sourceI, O /*sourceO*/) {
  return {{"sin(" + sourceI.e + ")"},
          {"sin(3.14159265358979323846-(" + sourceI.e + "))"}};
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = quich
//       Name = Double
// Description = quich( x+x )=quich( (x)*2 )
// ReferenceID =
vector<I> MR3(I sourceI, O /*sourceO*/) {
  return {{sourceI.e + "+" + sourceI.e}, {"(" + sourceI.e + ")*2"}};
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
  istringstream(line) >> word >> word >> ret.r;
  return ret;
}

void testPool() {
  ifstream pool("./inputs/t");
  assert(pool.is_open());
  int i = 1;
  int limit = 3;
  string input, word, scheme, id;
  I sourceI;
  O sourceO;
  while (getline(pool, input)) {
    cout << "Test " << i << '\t' << string(30, '*') << '\n';
    cout << "\tSource:" << endl;
    cout << "\t\t" << input << '\n';
    //        cout<<"\tOutput:\t"<<output.toStdString()<<'\n';
    sourceI.e.clear();
    istringstream(input) >> scheme >> id >> sourceI.e;
    //    sourceO = readO("./outputs/t" + id);
    for (int iMR = 1; iMR <= nMR; iMR++) {
      vector<I> follows = MR(iMR, sourceI, sourceO);
      cout << "\tMR" << iMR << ":" << endl;
      for (I &follow : follows)
        cout << "\t\t" << scheme << ' ' << id << ' ' << follow.e << endl;
    }
    limit--;
    if (limit == 0)
      break;
    i++;
  }
  pool.close();
}

int main(int argc, char *argv[]) {
//#define TEST
#ifndef TEST
  if (argc == 1) {
    printf("%d\n", nMR);
    return 0;
  }
  assert(argc == 5);
  string pathInputs(argv[1]);
  int iMR = atoi(argv[2]);
  string scheme, id;
  I input;
  istringstream(argv[3]) >> scheme >> id >> input.e;
  O output = readO(argv[4]);
  vector<I> follows = MR(iMR, input, output);
  if (!follows.empty()) {
    for (I &follow : follows)
      cout << "f " << id << ' ' << follow.e << endl;
  }
#else
  testPool();
#endif
  return 0;
}
