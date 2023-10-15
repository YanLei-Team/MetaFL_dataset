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
  vector<string> e;
};

template <typename T> istream &operator>>(istream &in, vector<T> &v) {
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
  double r;
};

// ------------------------------- MR 1 -------------------------------
//  ProgramID = clac
//       Name = Subtract self
// Description = clac(x x -)=0
// ReferenceID =
vector<I> MR1(I sourceI, O /*sourceO*/) {
  I follow = sourceI;
  follow.e.insert(follow.e.end(), sourceI.e.begin(), sourceI.e.end());
  follow.e.push_back("-");
  return {follow};
}

// ------------------------------- MR 2 -------------------------------
//  ProgramID = clac
//       Name = Sine
// Description = clac(x sin)=clac(pi x - sin)
// ReferenceID =
vector<I> MR2(I sourceI, O /*sourceO*/) {
  I follow1 = sourceI, follow2;
  follow1.e.push_back("sin");
  follow2.e.push_back("3.14159265358979323846");
  follow2.e.insert(follow2.e.end(), sourceI.e.begin(), sourceI.e.end());
  follow2.e.push_back("-");
  follow2.e.push_back("sin");
  return {follow1, follow2};
}

// ------------------------------- MR 3 -------------------------------
//  ProgramID = clac
//       Name = Double
// Description = clac(x dup +)=clac(x 2 *)
// ReferenceID =
vector<I> MR3(I sourceI, O /*sourceO*/) {
  I follow1 = sourceI, follow2 = sourceI;
  follow1.e.push_back("dup");
  follow1.e.push_back("+");
  follow2.e.push_back("2");
  follow2.e.push_back("*");
  return {follow1, follow2};
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
