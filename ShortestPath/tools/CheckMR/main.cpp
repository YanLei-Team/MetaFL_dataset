#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<vector>
#include<cassert>
#include<algorithm>
#include<map>
#include<sstream>
#include<random>

using namespace std;

#define random(x) (rd()%x)
random_device rd;

typedef long long ll;

struct I{
//    int n; // number of nodes
//    int s; // source node
//    int t; // target node
};

struct O{
    ll l; // length
    int c; // count
    vector<int> p; // path
};


struct Case {
    I i;
    O o;
    Case()=default;
    Case(I i,O o):i(i),o(o) {}
};


string readAll(string filename) {
    ifstream file(filename);
    assert(file.is_open());
    string all,line;
    while(getline(file,line))
        all.append(line);
    file.close();
    return all;
}

O readO(string filename){
    ifstream file(filename);
    O o;
    string word;
    file>>word>>o.l>>word>>o.c;
    int x;
    for(int i=0;i<o.c;i++){
        file>>x;
        o.p.push_back(x);
        if(i!=o.c-1) file>>word;
    }
    file.close();
    return o;
}

// ---------------------------------------------------------------- MR 1 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Reverse
//Description = |SP(i,j)| = |SP(j,i)|
//ReferenceID =
bool MR1(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.o.l==follows[0].o.l;
}

// ---------------------------------------------------------------- MR 2 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Triangle inequality
//Description = |SP(i,j)| <= |SP(i,k)|+|SP(k,j)|
//ReferenceID =
bool MR2(Case &source,vector<Case> &follows) {
    assert(follows.size()==2);
    return source.o.l<=follows[0].o.l+follows[1].o.l;
}

// ---------------------------------------------------------------- MR 3 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Optimal
//Description = |SP(i,j)| = |SP(i,k)|+|SP(k,j)| if k in SP(i,j)
//ReferenceID =
bool MR3(Case &source,vector<Case> &follows) {
    assert(follows.size()==2);
    return source.o.l==follows[0].o.l+follows[1].o.l;
}

vector<bool(*)(Case &,vector<Case> &)> MRs{MR1,MR2,MR3};
int nMR=MRs.size();

bool MR(int iMR,Case &source,vector<Case> &follows) {
    assert(1<=iMR && iMR<=nMR);
    return MRs[iMR-1](source,follows);
}

int main(int argc, char *argv[]) {
    if(argc==1) {
        printf("%d\n",nMR);
        return 0;
    }
    assert(argc>=6);
    string pathInputs(argv[1]);
    int iMR=atoi(argv[2]);
    string scheme,id;
    istringstream(argv[3])>>scheme>>id;
    Case source(I(),readO(argv[4]));
    vector<Case> follows;
    int nFollow=(argc-4)/2;
    for(int i=0; i<nFollow; i++){
        istringstream(argv[5+i*2])>>scheme>>id;
        follows.push_back(Case(I(),readO(argv[5+i*2+1])));
    }
    return MR(iMR,source,follows)?0:1;
}
