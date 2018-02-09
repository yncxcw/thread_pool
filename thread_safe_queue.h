#include<queue>
#include<mutex>
#include<iostream>

template<typename T>
class thread_safe_queue{

   std::mutex queue_lock;
   std::queue<T> safe_queue;

public:

   thread_safe_queue()=default;


   //not supported yet;
   thread_safe_queue(std::queue<T>& n_queue)=delete;


   void push(T& t){
     std::unique_lock<std::mutex> lock(queue_lock);
     safe_queue.push(t); 
   }

   
   bool empty(){
     std::unique_lock<std::mutex> lock(queue_lock);
     return safe_queue.empty();

   } 
   
   size_t size(){
    std::unique_lock<std::mutex> lock(queue_lock);
    return safe_queue.size();
   }
   
   void pop(){
     std::unique_lock<std::mutex> lock(queue_lock);
     safe_queue.pop();
   }

   T& front(){
     std::unique_lock<std::mutex> lock(queue_lock);
     return safe_queue.front();
   }

   T& back(){
     std::unique_lock<std::mutex> lock(queue_lock);
     return safe_queue.back();

   }
};
