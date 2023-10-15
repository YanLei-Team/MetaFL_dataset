#include <bits/stdc++.h>

using namespace std;

//#define DEBUG
typedef long long ll;

random_device rd;
ll random(ll m){return rd()%m;}
ll random(ll l,ll r){return l+rd()%(r-l+1);}

struct I{
    int e;
    vector<string> T;
    string p;
    I(int e=3,vector<string> T=vector<string>(),string p=string()):e(e),T(T),p(p){}
    bool operator== (const I &o) const { return e==o.e && p==o.p && T==o.T; }
    bool operator!= (const I &o) const { return e!=o.e || p!=o.p || T!=o.T; }
};


struct O{
    vector<string> M;
};

// ---------------------------------------------------------------- MR 1 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Concatenation of some elements of P to t_s
//Description = 取短序列集合P的一个非空子集，将其中的元素连续拼接到t_s尾部
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
I MR1(I sourceI,O /*sourceO*/) {
    I followI=sourceI;
    int nT=sourceI.T.size();
    vector<int> selected(nT);
    for(int i=0;i<nT;i++) selected[i]=i;
    for(int i=0;i<nT;i++) swap(selected[i],selected[random(nT)]);
    int nSelected=random(1,nT);
    for(int i=0;i<nSelected;i++)
        followI.p.append(followI.T[selected[i]]);
    return followI;
}

// ---------------------------------------------------------------- MR 2 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Changing of e_s
//Description = 改变e_s取值，使输出成为原来的子集或超集
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
const int MIN_E_N=0;
const int MAX_E_N=4;
I MR2(I sourceI,O /*sourceO*/) {
    if(sourceI.e==MIN_E_N || sourceI.e==MAX_E_N) return sourceI;
    I followI=sourceI;
    followI.e=(followI.e+random(1,4))%5;
    return followI;
}

// ---------------------------------------------------------------- MR 3 ---------------------------------------------------------------
//  ProgramID = SeqMap
//       Name = Deletion of substrings in t_s
//Description = 从t_s的首部或尾部删除连续子串
//ReferenceID = Metamorphic slice: An application in spectrum-based fault localization
I MR3(I sourceI,O /*sourceO*/) {
    I followI=sourceI;
    int lenP=followI.p.size();
    int lenCut=random(1,lenP-1);
    followI.p=followI.p.substr(random(2) ? 0 : lenCut,lenP-lenCut);
    return followI;
}



vector<I(*)(I,O)> MRs {MR1,MR2,MR3};
int nMR=MRs.size();

I MR(int iMR,I input,O output) {
    assert(1<=iMR && iMR<=nMR);
    return MRs[iMR-1](input,output);
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

void writeP(string filename,string p){
    ofstream file(filename,ios::out);
    file<<">1\n"<<p<<endl;
    file.close();
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

void writeT(string filename,vector<string> T){
    ofstream file(filename,ios::out);
    for(size_t i=0;i<T.size();i++)
        file<<'>'<<i+1<<'\n'<<T[i]<<endl;
    file.close();
}

void printI(I i,string pre){
    cout<<pre<<"e = "<<i.e<<endl;
    cout<<pre<<"T = [ ";
    for(size_t iT=0;iT<i.T.size();iT++)
        cout<<(iT==0?"":pre+"      ")<<i.T[iT]<<(iT==i.T.size()-1?" ]":",")<<'\n';
    cout<<pre<<"p = "<<i.p<<endl;
};

void testPool() {
    ifstream pool("./inputs/TestPool_all");
    assert(pool.is_open());
    int i=1;
    const int limit=3;
    string line;
    string id;
    I input;
    O output;
    while(pool>>input.e>>id){
//        output=readAll(string("./outputs/t")+to_string(i));
        input.T=readT("./inputs/T/"+id);
        input.p=readP("./inputs/p/"+id);
        cout<<"Test "<<i<<'\t'<<string(30,'*')<<'\n';
//        cout<<"\tSource:\t"<<input<<'\n';
        cout<<"    Source:\n";
        printI(input,"        ");
        for(int iMR=1; iMR<=nMR; iMR++){
            cout<<"    MR"<<iMR<<":\n";
            printI(MR(iMR,input,output),"        ");
        }
        if((++i)>limit)
            break;
    }
    pool.close();
}

int main(int argc, char *argv[]) {
#ifndef DEBUG
    if(argc==1) {
        printf("%d\n",nMR);
        return 0;
    } else if(argc==5){
        /*
        testcase_f = check_output([
            0: executable,
            1: str(self.path_inputs),
            2: str(i_mr),
            3: testcase_s,
            4: str(self.path_oracles / 's' / ("t%d" % i_testcase))
        ])
         */
        string pathInputs(argv[1]);
        int iMR=atoi(argv[2]);
        string line(argv[3]);
        string scheme;
        int e;
        string id;
        istringstream(line)>>scheme>>e>>id;
        I input(e,readT(pathInputs+"/s/T/"+id),readP(pathInputs+"/s/p/"+id));
        O output;
        I follow=MR(iMR,input,output);
        if(follow!=input){
            id=id+"_"+to_string(iMR);
            writeT(pathInputs+"/f/T/"+id,follow.T);
            writeP(pathInputs+"/f/p/"+id,follow.p);
            cout<<"f "<<follow.e<<' '<<id<<endl;
        }
    }
#else
    testPool();
#endif
    return 0;
}
