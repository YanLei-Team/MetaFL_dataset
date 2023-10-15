#include <bits/stdc++.h>

using namespace std;

struct I{
    int e;
    vector<string> T;
    string p;
    I(int e=3,vector<string> T=vector<string>(),string p=string()):e(e),T(T),p(p){}
    bool operator== (const I &o) const { return e==o.e && p==o.p && T==o.T; }
    bool operator!= (const I &o) const { return e!=o.e || p!=o.p || T!=o.T; }
};


struct O{
    vector<int> M;
    O(vector<int> M=vector<int>()):M(M){}
};

bool belong(vector<int> l,vector<int> r,bool subset=false){
    if(l.size()>r.size() || (subset && l.size()==r.size())) return false;
    sort(l.begin(),l.end());
    sort(r.begin(),r.end());
    for(size_t i=0;i<l.size();i++)
        if(l[i]!=r[i]) return false;
    return true;
}

struct Case {
    I i;
    O o;
    Case()=default;
    Case(I i,O o):i(i),o(o) {}
};

// ---------------------------------------------------------------- MR 1 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Concatenation of some elements of P to t_s
//Description = 取短序列集合P的一个非空子集，将其中的元素连续拼接到t_s尾部
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
bool MR1(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    Case &follow=follows[0];
    return belong(source.o.M,follow.o.M,true);
}

// ---------------------------------------------------------------- MR 2 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Changing of e_s
//Description = 改变e_s取值，使输出成为原来的子集或超集
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
bool MR2(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    Case &follow=follows[0];
    return source.i.e<follow.i.e
            ? belong(source.o.M,follow.o.M)
            : belong(follow.o.M,source.o.M);
}

// ---------------------------------------------------------------- MR 3 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Deletion of substrings in t_s
//Description = 从t_s的首部或尾部删除连续子串
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
bool MR3(Case &source,vector<Case> &follows) {
    assert(follows.size()==1);
    Case &follow=follows[0];
    return belong(follow.o.M,source.o.M);
}

vector<bool(*)(Case &,vector<Case> &)> MRs{MR1,MR2,MR3};
int nMR=MRs.size();

bool MR(int iMR,Case &source,vector<Case> &follows) {
    assert(1<=iMR && iMR<=nMR);
    return MRs[iMR-1](source,follows);
}

string readAll(string filename) {
    ifstream file(filename);
    assert(file.is_open());
    string all,line;
    while(getline(file,line))
        all.append(line);
    file.close();
    return all;
}

string readP(string filename){
    ifstream file(filename);
    assert(file.is_open());
    string line;
    getline(file,line);
    getline(file,line);
    file.close();
    return line;
}

vector<string> readT(string filename){
    ifstream file(filename);
    assert(file.is_open());
    string line;
    vector<string> T;
    while(getline(file,line)){
        getline(file,line);
        T.push_back(line);
    }
    return T;
}

vector<int> readM(string filename){
    ifstream file(filename);
    assert(file.is_open());
    string line;
    vector<int> M;
    int id;
    getline(file,line);
    while(file>>id){
        M.push_back(id);
        getline(file,line);
    }
    return M;
}

void printI(I i,string pre){
    cout<<pre<<"e = "<<i.e<<endl;
    cout<<pre<<"T = [ ";
    for(size_t iT=0;iT<i.T.size();iT++)
        cout<<(iT==0?"":pre+"      ")<<i.T[iT]<<(iT==i.T.size()-1?" ]":",")<<'\n';
    cout<<pre<<"p = "<<i.p<<endl;
};

int main(int argc, char *argv[]) {
    if(argc==1) {
        printf("%d\n",nMR);
        return 0;
    }
    assert(argc>=6);
    string pathInputs(argv[1]);
    int iMR=atoi(argv[2]);
    string scheme;
    int e;
    string id;
    istringstream(argv[3])>>scheme>>e>>id;
    Case source(
        I(e,readT(pathInputs+"/s/T/"+id),readP(pathInputs+"/s/p/"+id)),
        O(readM(argv[4]))
    );
    vector<Case> follows;
    int nFollow=(argc-5)/2;
    for(int i=0; i<nFollow; i++){
        istringstream(argv[5+i*2])>>scheme>>e>>id;
        follows.push_back(Case(
            I(e,readT(pathInputs+"/f/T/"+id),readP(pathInputs+"/f/p/"+id)),
            O(readM(argv[5+i*2+1]))
        ));
    }
    return MR(iMR,source,follows)?0:1;
}
