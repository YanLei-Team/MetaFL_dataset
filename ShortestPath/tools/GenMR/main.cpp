#include<iostream>
#include<fstream>
#include<string>
#include<random>
#include<algorithm>
#include<regex>
#include<cassert>
#include<cctype>

using namespace std;

#define random(x) (rd()%x)
random_device rd;

typedef long long ll;

struct I{
    int n; // number of nodes
    int s; // source node
    int t; // target node
};

struct O{
    ll l; // length
    int c; // count
    vector<int> p; // path
};

// ---------------------------------------------------------------- MR 1 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Reverse
//Description = |SP(i,j)| = |SP(j,i)|
//ReferenceID =
vector<I> MR1(I sourceI,O /*sourceO*/) {
    I follow=sourceI;
    swap(follow.s,follow.t);
    return vector<I>{follow};
}

// ---------------------------------------------------------------- MR 2 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Triangle inequality
//Description = |SP(i,j)| <= |SP(i,k)|+|SP(k,j)|
//ReferenceID =
vector<I> MR2(I sourceI,O /*sourceO*/) {
    int k=random(sourceI.n);
    vector<I> follows{sourceI,sourceI};
    follows[0].t=k;
    follows[1].s=k;
    return follows;
}


// ---------------------------------------------------------------- MR 3 ---------------------------------------------------------------
//  ProgramID = ShortestPath
//       Name = Optimal
//Description = |SP(i,j)| = |SP(i,k)|+|SP(k,j)| if k in SP(i,j)
//ReferenceID =
vector<I> MR3(I sourceI,O sourceO) {
    int k=sourceO.p[random(sourceO.p.size())];
    vector<I> follows{sourceI,sourceI};
    follows[0].t=k;
    follows[1].s=k;
    return follows;
}


vector<vector<I>(*)(I,O)> MRs {MR1,MR2,MR3};
int nMR=MRs.size();

vector<I> MR(int iMR,I input,O output) {
    assert(1<=iMR && iMR<=nMR);
    return MRs[iMR-1](input,output);
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

vector<string> readLines(string filename) {
    ifstream file(filename);
    assert(file.is_open());
    string line;
    vector<string> all;
    while(getline(file,line)){
        line.append(1,'\n');
        all.push_back(line);
    }
    file.close();
    return all;
}
void writeLines(string filename,vector<string> lines){
    ofstream file(filename);
    for(string &line:lines)
        file<<line;
    file.close();
}

void testPool() {
    ifstream pool("./inputs/t");
    assert(pool.is_open());
    int i=1;
    int limit=10;
    string input,word,scheme,id;
    I sourceI;
    O sourceO;
    while(getline(pool,input)){
        cout<<"Test "<<i<<'\t'<<string(30,'*')<<'\n';
        cout<<"\tSource:"<<endl;
        cout<<"\t\t"<<input<<'\n';
//        cout<<"\tOutput:\t"<<output.toStdString()<<'\n';
        istringstream(input)>>scheme>>id>>sourceI.s>>sourceI.t;
        sourceO=readO("./outputs/t"+id);
        for(int iMR=1; iMR<=nMR; iMR++){
            sourceI.n=sourceI.t+1;
            vector<I> follows=MR(iMR,sourceI,sourceO);
            cout<<"\tMR"<<iMR<<":"<<endl;
            for(I& follow:follows)
                cout<<"\t\t"<<scheme<<' '<<id<<' '<<follow.s<<' '<<follow.t<<endl;
        }
        limit--;
        if(limit==0) break;
        i++;
    }
    pool.close();
}

int main(int argc, char *argv[]) {
    if(argc==1) {
        printf("%d\n",nMR);
        return 0;
    }
    assert(argc==5);
    string pathInputs(argv[1]);
    int iMR=atoi(argv[2]);
    string scheme,id;
    I input;
    istringstream(argv[3])>>scheme>>id>>input.s>>input.t;
    input.n=input.t+1;
    O output=readO(argv[4]);
    vector<I> follows=MR(iMR,input,output);
    if(!follows.empty()){
        for(I& follow:follows)
            cout<<"s "<<id<<' '<<follow.s<<' '<<follow.t<<endl;
    }

//    testPool();

    return 0;
}
