#include<thread>
#include<iostream>
#include"thread_safe_queue.h"
#include<functional>
#include<mutex>
#include<condition_variable>
#include<future>
//define worker



class thread_pool{

//iner class
class worker{

  int w_id;
  thread_pool* t_pool;

public:
  worker(int _id,thread_pool* _pool):w_id(_id),t_pool(_pool){

  }

  void operator()(){
     std::function<void()> func;
     while(!t_pool->is_stop){
     //keep the variable lock
     {
     std::unique_lock<std::mutex> lock(t_pool->c_variable_mutex);
     if(t_pool->function_queue.empty())
          t_pool->c_variable.wait(lock);
     

     //at this point, function_queue should have functions
     //this is very importan, to tell if the queue is empty
     //think of a case, when the queue is empty but the is_stop is till ture
     //check empty to ensure to not call front()
     if(t_pool->function_queue.empty())
        continue;
     func=t_pool->function_queue.front();
     t_pool->function_queue.pop();
     }
     //std::cout<<w_id<<" process task address "<<&func<<std::endl;
     //execute the function
     try{
      func();
     }catch(std::exception& e){
     std::cout<<"function exception "<<e.what()<<std::endl;
     break;
     }

    }
  }

};


 bool is_stop;
 int pool_size;
 thread_safe_queue<std::function<void()>> function_queue;
 std::condition_variable c_variable;
 std::mutex c_variable_mutex;
 std::vector<std::thread> threads;
 
 public:
   thread_pool(int pool_size):is_stop(false), pool_size(pool_size){
   
   }
   thread_pool() = delete;
   thread_pool(const thread_pool& ) = delete;
   thread_pool(thread_pool&& ) = delete;

   void start(){
    for(int i=0;i < pool_size ;i++)
      threads.push_back(std::thread(worker(i,this)));
   }

   int thread_pool_size(){
     return pool_size;
   }

   
  void stop(){
    is_stop=true; 
    c_variable.notify_all();
    std::cout<<"thread pool stop"<<std::endl;
    for(int i=0;i<threads.size();i++){
         if(threads[i].joinable())
             threads[i].join();
    }

  }


   template<typename F, typename... Args>
   auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>{
      using return_type=typename std::result_of<F(Args...)>::type;
      std::function<return_type()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
      //using shared ptr
      auto task_ptr=std::make_shared<std::packaged_task<return_type()>>(func);
      //wrap task into void function with lambda expression
      std::function<void()> void_function=[task_ptr](){
        (*task_ptr)(); 
      };

      //std::cout<<"insert address "<<&void_function<<std::endl;
      function_queue.push(void_function);
      c_variable.notify_one();
      return task_ptr->get_future();

      
   }


};

