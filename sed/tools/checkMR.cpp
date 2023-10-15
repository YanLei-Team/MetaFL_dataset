#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<vector>
#include<cassert>

using namespace std;

struct Case {
    string input,output;
    Case()=default;
    Case(string i,string o):input(i),output(o) {}
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

bool MR1(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.output==follows[0].output;
}

bool MR2(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.output==follows[0].output;
}

bool MR3(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.output==follows[0].output;
}

bool MR4(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.output==follows[0].output;
}

bool MR4rev(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    return source.output==follows[0].output;
}

vector<bool(*)(Case &,vector<Case> &)> MRs{/*MR1,*/MR2,MR3,MR4,MR4rev};
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
    assert(argc>=7);
    string pathInputs(argv[1]);
    int iMR=atoi(argv[2]);
    Case source(argv[3],readAll(argv[4]));
    vector<Case> follows;
    int nFollow=(argc-5)/2;
    for(int i=0; i<nFollow; i++)
        follows.push_back(Case(argv[5+i*2],readAll(argv[5+i*2+1])));
    return MR(iMR,source,follows)?0:1;
}
