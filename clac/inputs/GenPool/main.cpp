#include <iostream>
#include <cstdio>
#include <random>
#include <functional>
#include <chrono>

using namespace std;
typedef long long ll;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);

uniform_int_distribution<ll> distributionInt;
int randInt(ll l,ll r){ return uniform_int_distribution<ll>(l,r)(generator); }

normal_distribution<double> distributionReal;
double randReal(double mu,double std){ return normal_distribution<double>(mu,std)(generator); }

string randOperand(){
    return to_string(randReal(5.0,5.0));
}

vector<string> unaryOperators={"ln","log","sin","cos","tan","erf","ceil","floor","round","abs"};
string randUnaryOperator(){
    return unaryOperators[randInt(0,unaryOperators.size()-1)];
}

vector<string> binaryOperators={"+","-","*","/","%","drop"};
string randBinaryOperator(){
    return binaryOperators[randInt(0,binaryOperators.size()-1)];
}

vector<string> randExpresion(int depth){
    if(depth==0) return {};
    if(depth==1 && randInt(0,1)) return {randOperand()};
    int dL=randInt(0,depth),dR=depth-dL;
    vector<string> exp;
    vector<string> expL=randExpresion(dL);
    vector<string> expR=randExpresion(dR);
    exp.reserve(expL.size()+expR.size()+1);
    exp.insert(exp.end(),expL.begin(),expL.end());
    exp.insert(exp.end(),expR.begin(),expR.end());
    if(dL==0 || dR==0) exp.push_back(randUnaryOperator());
    else exp.push_back(randBinaryOperator());
    return exp;
}

void printExpression(vector<string> e){
    for(unsigned i=0;i<e.size();i++)
        cout<<e[i]<<(i==e.size()-1?"\n":" ");
}

int main() {
    int N=1e4;
    int minD=1,maxD=12;
    while(N--)
        printExpression(randExpresion(randInt(minD,maxD)));
    return 0;
}
