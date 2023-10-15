#include<bits/stdc++.h>
using namespace std;

typedef long long ll;

const int nA=4;
const char A[nA+1]="ACGT";

const int MIN_N_E=0;
const int MED_N_E=3;
const int MAX_N_E=4;
const int MIN_N_T=1;
const int MAX_N_T=5;
const int MIN_L_T=10;
const int MAX_L_T=20;
const int MIN_N_P=1;
const int MAX_N_P=8;

random_device rd;

ll random(ll m){return rd()%m;}
ll random(ll l,ll r){return l+rd()%(r-l+1);}

int generateE(int low,int mid,int high){
  return random(2) ? mid : (random(2) ? random(low,mid) : random(mid,high));
}

vector<string> generateT(int min_n_t,int max_n_t,int min_l_t,int max_l_t){
  int nT=random(min_n_t,max_n_t);
  vector<string> T;
  for(int i=0,nL;i<nT;i++){
    nL=random(min_l_t,max_l_t);
    string t(nL,A[0]);
    for(int j=0;j<nL;j++)
      t[j]=A[random(nA)];
    T.push_back(t);
    //cout<<"["<<i+1<<"]"<<t<<endl;
  }
  return T;
}

string generateP(vector<string> T,int pNorm=100,int pSub=1,int pIns=1,int pDel=1){
  int n=random(MIN_N_P,MAX_N_P);
  string p="";
  for(int i=0;i<n;i++)
    p+=T[random(T.size())];
  stringstream ss;
  int sNorm=0;
  int sSub=sNorm+pNorm;
  int sIns=sSub+pSub;
  int sDel=sIns+pIns;
  int s=sDel+pDel;
  for(size_t i=0;i<p.size();){
    int dice=random(s*10)/10;
    if(dice>=sDel){
      i++;
    }else if(dice>=sIns){
      ss<<A[random(nA)];
    }else if(dice>=sSub){
      ss<<A[random(nA)];
      i++;
    }else{
      ss<<p[i];
      i++;
    }
  }
  return ss.str();
}

int main(void){
  int n=10;
  char filename[100];
  ofstream("TestPool_all",ofstream::out).close();
  for(int i=1;i<=n;i++){
    ofstream filePool("TestPool_all",ofstream::app);
    filePool<<generateE(MIN_N_E,MED_N_E,MAX_N_E)<<' '<<i<<endl;
    filePool.close();
    vector<string> T=generateT(MIN_N_T,MAX_N_T,MIN_L_T,MAX_L_T);
    sprintf(filename,"T/%d",i);
    ofstream fileT(filename,ofstream::out);
    for(size_t j=0;j<T.size();j++)
      fileT<<'>'<<j+1<<'\n'<<T[j]<<endl;
    fileT.close();
    //cout<<"len(T)="<<T.size()<<endl;
    vector<string> noise=generateT(MIN_N_T,MAX_N_T,1,MAX_L_T);
    //cout<<"len(noise)="<<noise.size()<<endl;
    T.insert(T.end(),noise.begin(),noise.end());
    //cout<<"len(T+noise)="<<T.size()<<endl;
    string p=generateP(T,46,2,1,1);
    sprintf(filename,"p/%d",i);
    ofstream fileP(filename,ofstream::out);
    fileP<<">1\n"<<p<<endl;
    fileP.close();
  }
  return 0;
}
