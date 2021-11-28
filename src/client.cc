#include <iostream>
#include <string>
#include "cmdline.h"
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mutex>
#include <pthread.h>
#include <atomic>
#include <vector>
#include <chrono>

#define SERV_PORT 8081
#define SERVER_ADDR "127.0.0.1"

std::string content = "";
long long num_msg = 0;

bool run = false;
std::mutex global_mutex;
std::mutex time_mutex;
std::vector<double> times_vec;
std::atomic<long long> num_msg_atomic(0);

class Worker {
 public:
  Worker(unsigned int id): id_(id) {
    std::cout << "worker " << id_ << " is ready" << std::endl;
  }
  ~Worker();
  int apply_work(){
    std::lock_guard<std::mutex> locker(global_mutex);
    std::cout << "worker " << id_ << " num_msg " << num_msg << std::endl;
    num_msg -= 1;
    return num_msg;
  }
  int add_time(){
    std::lock_guard<std::mutex> locker(time_mutex);

  }
  int apply_work_atomic(){
    if (num_msg_atomic-- > 0)
      std::cout << "worker " << id_ << " num_msg " << num_msg_atomic << std::endl;
    return num_msg_atomic;
  }

  void work() {
    while (!run) {
     // wait to run
    }
    while (num_msg_atomic > 0){
      if (apply_work() < 0) return;
      int sock = -1;
      struct sockaddr_in serv_addr;
      sock = socket(PF_INET, SOCK_STREAM, 0);
      if (sock < 0) {
        std::cout << id_ << " socket init error" << std::endl;
        return;
      }
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(SERV_PORT);
      serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
      int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof (struct sockaddr));
      if (ret < 0) {
        std::cout << id_ << " connect error" << std::endl;
        return;
      }
      const char* buffer = content.c_str();
      int len = send(sock, buffer, strlen(buffer), 0);
      recv(sock, nullptr, 0, 0);

      close(sock);
   }
 }
 private:
  unsigned int id_;
  double time_response;
};


int main(int argc, char** argv) {
  // setup parser
  cmdline::parser par;
  par.add<unsigned int>("concurrency", 'c', "number of concurrency", true, 1, cmdline::range(1, 1024));
  par.add<long long>("number", 'n', "total number of tcp messages", true, 1, cmdline::range<long long>(1, __LONG_LONG_MAX__));
  par.add<std::string>("string", 's', "message to be sent", false, "hello, tcp!");
  // parsing
  par.parse_check(argc, argv);
  // get cmdline parameter
  unsigned int concur = par.get<unsigned int>("concurrency");
  num_msg = par.get<long long>("number");
  num_msg_atomic = num_msg;
  content = par.get<std::string>("string");
  
  // do
  std::cout << "concurrency: " << concur << std::endl
            << "number of tcp messages: " << num_msg << std::endl
            << "message content: " << content << std::endl;
  run = false;
  for (unsigned int i = 0; i < concur; ++i){
    Worker* p_w = new Worker(i);
    std::thread t(&Worker::work, p_w);
    t.detach();
  }
  run = true;
  pthread_exit(NULL);
  return 0;
}
