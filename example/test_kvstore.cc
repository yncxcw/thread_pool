#define ITEM_SIZE 1024*1024
#define ITEM_NUME 50000
#include<map>
#include<iostream>
#include<chrono>
#include<random>
#include<future>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<fstream>
#include"../thread_pool.h"
std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> dis(0, ITEM_NUME-1);


class query_store{

//# of items, each item is 1M
int size;

//map from id to item
std::map<int, char*> _store;

public:

   query_store(int _size):size(_size){
      for(int i=0; i<size;i++){
        char* _pitem= new char[ITEM_SIZE];
        memset(_pitem,'a',ITEM_SIZE);
        _store[i]=_pitem;
      }
   } 


  void query(int index){
    if(index > size)
       return;

    char* _pitem=_store[index];
    //to evalute the effects of swapping, we do access the store
    for(int i=0;i<ITEM_SIZE;i++)
       char a=_pitem[i];

  } 


  ~query_store(){
   for(int i=0; i<size; i++)
     delete _store[i]; 


  }

};




double wrapper_access(query_store* store,int idx){
auto started = std::chrono::high_resolution_clock::now();
store->query(idx);
auto done = std::chrono::high_resolution_clock::now();
double elapsed=(double)std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count();

return elapsed;
}


double test_aplusb(query_store* store, int index){
auto started = std::chrono::high_resolution_clock::now();
double c=1.0+2.0;
auto done = std::chrono::high_resolution_clock::now();
double elapsed=(double)std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count();

return elapsed;

}


int main(){

std::cout<<"initialize store"<<std::endl;
//query store
query_store* store=new query_store(ITEM_NUME);

//thread pool
std::cout<<"initialize thread pool"<<std::endl;
thread_pool pool(10);
pool.start();
std::vector<double> runtimes;

double a=0;

std::cout<<"start query"<<std::endl;
//control the # of iterations
for(int i=0; i<300;i++){
 //for each iteration, we submit 2000 queries
 std::vector<std::future<double>> results;
 for(int j=0; j<2000; j++){
   int idx=dis(gen);
   results.push_back(pool.submit(wrapper_access, store,idx));
  }

 //syn for the results
 for(auto& thread : results){
  runtimes.push_back(thread.get());
 }
}

pool.stop();
delete store;
std::cout<<"finish "<<std::endl;
std::sort(runtimes.begin(),runtimes.end());
std::ofstream fout("latency",std::ofstream::out);
int i=0;
while(i<0.999*runtimes.size()){
   fout<<runtimes[i]<<std::endl;
   i++;
}
std::cout<<"1st "<<runtimes[(int)(runtimes.size()*0.5)]<<" nst "<<runtimes[(int)(runtimes.size()*0.95)]<<std::endl;
return 0;
}
