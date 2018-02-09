#include<iostream>
#include<vector>
#include<queue>
void func(){

int *p=new int;
*p=10;

std::cout<<"address "<<p<<std::endl;

}


int main(){

for(int i=0;i<10;i++)
  func();

std::queue<int> vecs;

int a=vecs.front();

std::cout<<a<<std::endl;

return 0;

}
