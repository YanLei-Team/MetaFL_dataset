#include<iostream>
#include<fstream>
#include<string>
#include<random>
#include<algorithm>
#include<regex>
#include<cassert>

using namespace std;

#define random(x) (rd()%x)
random_device rd;

// ---------------------------------------------------------------- MR 1 ---------------------------------------------------------------
//   ProgramID: grep
//        Name: Decomposing the bracketed sub-expression
// Description: 将"[a-f]"这种范围集合模式展开成"[abcdef]"这种字符集合模式
// ReferenceID: On the Correlation between the Effectiveness of Metamorphic Relations and Dissimilarities of Test Case Executions
string MR1(string sourceI,string /*sourceO*/) {
    regex e(
        "(\\[\\^?)"
        "(([[:alnum:]]-[[:alnum:]])+)"
        "(\\-?\\])"
    );
    smatch m;
    bool found=false;
    string follow;
    while(regex_search(sourceI,m,e)){
        if(!found) found=true;
        follow.append(m.prefix());
        follow.append(m[1]);
        string ranges=m[2];
        int nRange=ranges.size()/3,sz;
        char lc,rc;
        for(int i=0; i<nRange; i++) {
            lc=ranges[i*3];
            rc=ranges[i*3+2];
            if(rc<lc) swap(lc,rc);
            sz=rc-lc+1;
            if((isdigit(lc) && isdigit(rc))
                    ||(isupper(lc) && isupper(rc))
                    ||(islower(lc) && islower(rc))) {
                string cset(sz,' ');
                for(int i=0; i<sz; i++)
                    cset[i]=lc+i;
                shuffle(cset.begin(),cset.end(),default_random_engine(rd()));
                follow.append(move(cset));
            } else
                follow.append(ranges);
        }
        follow.append(m[4]);
        sourceI=m.suffix();
    }
    follow.append(sourceI);
    return follow;
}

// ---------------------------------------------------------------- MR 2 ---------------------------------------------------------------
//   ProgramID: grep
//        Name: Splitting the bracketed structure
// Description: 将"[abcdef]"或"[a-f]"这种集合模式分解成"[ab]|[c-f]"类似的模式
// ReferenceID: Metamorphic slice: An application in spectrum-based fault localization
string MR2(string sourceI,string /*sourceO*/) {
    regex e(
        "(\\[)"
        "([[:alnum:]]-[[:alnum:]])"
        "(\\])"
    );
    bool found=false;
    smatch m;
    string follow;
    while(regex_search(sourceI,m,e)){
        if(!found) found=true;
        follow.append(m.prefix());
        follow.append("(");
        follow.append(m[1]);
        string range=m[2];
        char lc=range[0];
        char rc=range[2];
        if(rc<lc) swap(lc,rc);
        int sz=rc-lc+1;
        if(sz>=2 && ((isdigit(lc) && isdigit(rc))
                     ||(isupper(lc) && isupper(rc))
                     ||(islower(lc) && islower(rc)))) {
            char mid=lc+max(1,int(random(sz)));
            int scheme=random(4);
            if(scheme%2) { // ab
                string cset(mid-lc,' ');
                for(int i=0; i<cset.size(); i++) cset[i]=lc+i;
                follow.append(cset);
            } else { // a-b
                follow.append(1,lc);
                follow.append(1,'-');
                follow.append(1,mid-1);
            }
            follow.append("]|[");
            if(scheme/2) { // cf
                string cset(rc+1-mid,' ');
                for(int i=0; i<cset.size(); i++) cset[i]=mid+i;
                follow.append(cset);
            } else { // c-f
                follow.append(1,mid);
                follow.append(1,'-');
                follow.append(1,rc);
            }
        } else
            follow.append(range);
        follow.append(m[3]);
        follow.append(")");
        sourceI=m.suffix();
    }
    follow.append(sourceI);
    return follow;
}

bool replace(string& str, const string& from, const string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// ---------------------------------------------------------------- MR 3 ---------------------------------------------------------------
//   ProgramID: grep
//        Name: Replace range operator with predefined class
// Description: 比如将"[0-9]"替换成"[[:digit:]]"
// ReferenceID: On the Correlation between the Effectiveness of Metamorphic Relations and Dissimilarities of Test Case Executions
string MR3(string sourceI,string /*sourceO*/) {
    vector<pair<string,string>> rangeOperators= {
        {"[0-9]","[[:digit:]]"},
        {"[a-z]","[[:alpha:]]"},
        {"[A-Z]","[[:alpha:]]"},
        {"[a-zA-Z]","[[:alpha:]]"},
    };
    for(auto &rangeOperator:rangeOperators)
        replace(sourceI,rangeOperator.first,rangeOperator.second);
    return sourceI;
}

// ---------------------------------------------------------------- MR 3 rev ---------------------------------------------------------------
//   ProgramID: grep
//        Name: Replace predefined class with range operator
// Description: 比如将"[[:digit:]]"替换成"[0-9]"
// ReferenceID: On the Correlation between the Effectiveness of Metamorphic Relations and Dissimilarities of Test Case Executions
string MR3rev(string sourceI,string /*sourceO*/) {
    vector<pair<string,string>> rangeOperators= {
        {"[:alnum:]","0-9A-Za-z"},
        {"[:digit:]","0-9"},
        {"[:xdigit:]","0-9a-fA-F"},
        {"[:lower:]","a-z"},
        {"[:upper:]","A-Z"},
        {"[:alpha:]","a-zA-Z"},
        {"[:blank:]"," \t"},
    };
    for(auto &rangeOperator:rangeOperators)
        replace(sourceI,rangeOperator.first,rangeOperator.second);
    return sourceI;
}


vector<string(*)(string,string)> MRs {MR1,MR2,MR3,MR3rev};
int nMR=MRs.size();

string MR(int iMR,string input,string output) {
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

void testPool() {
    ifstream pool("./inputs/t");
    assert(pool.is_open());
    int i=1;
    int limit=10;
    string input,output;
    while(getline(pool,input)){
        output=readAll(string("./outputs/t")+to_string(i));
        cout<<"Test "<<i<<'\t'<<string(30,'*')<<'\n';
        cout<<"\tSource:\t"<<input<<'\n';
//        cout<<"\tOutput:\t"<<output.toStdString()<<'\n';
        for(int iMR=1; iMR<=nMR; iMR++)
            cout<<"\tMR"<<iMR<<":\t"<<MR(iMR,input,output)<<'\n';
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
    string input(argv[3]);
    string output=readAll(argv[4]);
    string follow=MR(iMR,input,output);
    if(follow!=input) cout<<follow;

//    testPool();

    return 0;
}
