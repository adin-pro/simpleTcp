#include <iostream>
#include <string>
#include <string.h>
#include <memory>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>

#define PORT 8081
#define TCP_MAXLEN 1472

using std::string;

int main(int argc, char const *argv[])
{
  int server_fd, conn_fd;
  struct sockaddr_in address, client_address;
  int opt = 1;
  int addrlen = sizeof(address);
  socklen_t client_addr_len;
  char recv_buffer[TCP_MAXLEN] = {0};
  std::fstream log;
  char ACK[5] = "ACK";

  // Opening log file
  log.open("tcp.txt", std::ios::app);
  log << "aa" << std::endl;
  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
      
  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );
      
  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address, 
                                sizeof(address))<0){
      perror("bind failed");
      exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 128) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
  }
  int cnt = 0;
  while (true) {
    conn_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addr_len);
    if (conn_fd >= 0) {
      pid_t pid = fork();
      if (pid == 0){
        // child
        close(server_fd);
        recv(conn_fd, recv_buffer, sizeof (recv_buffer), 0);
        time_t t;
        t = time(NULL);
        std::cout << "\"recv " << inet_ntoa(client_address.sin_addr) 
            << " msg, msgid:" << t
            << ", content:" << recv_buffer << "\""<< std::endl;
        send(conn_fd, "ACK", 5, 0);
        close(conn_fd);
        exit(0);
      }else{
        // parent
        
      }
      close(conn_fd); 
    }    
  }
  return 0;
}

