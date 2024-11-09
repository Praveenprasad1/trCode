/*
**$Log$
*/
#include <iostream>
#include <thread>
#include <mutex>
//#include <pthread.h>

std::mutex mtx;
//pthread_mutex_t pmtx = PTHREAD_MUTEX_INITIALIZER;
const unsigned int totalThreads = 55;
int tstatus[totalThreads];
void* inc(void *i) {
 std::lock_guard<std::mutex> lock(mtx);
// pthread_mutex_lock(&pmtx);
 int id = *((int*)i);
 static int cnt;
 std::cout <<" id = "<<id<<std::endl;
 tstatus[id] = ++cnt;
// pthread_mutex_unlock(&pmtx);
 return (void *)&tstatus[id];
}

int main() {
 //pthread_t tid[totalThreads];
  std::thread *tid[totalThreads];
 int tval[totalThreads];
 for (int x = 0; x < totalThreads; x++)
  tstatus[x] = 0;
 for (int x = 0; x < totalThreads; x++) {
  tval[x] = x;
  //pthread_create(&tid[0], nullptr, inc, &tval[x]);
  tid[x] = new std::thread(inc, &tval[x]);
 }
// int status = -1;
 void *ptr;
 //void *ptr = (void *) &status;
 for (int x = 0; x < totalThreads; x++) {
 // if(!pthread_join(tid[x], &ptr)) {
//   std::cout <<"status = "<< *((int *)ptr)<<std::endl;
//  }else{
//   std::cout <<"Error for tid["<<x<<"]"<<std::endl;
//  }
  //(*tid[x]).join();
  tid[x]->join();
 }
return 0;
}
