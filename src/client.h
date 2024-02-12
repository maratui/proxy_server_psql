#ifndef TCP_PROXY_SERVER_CLIENT_H_
#define TCP_PROXY_SERVER_CLIENT_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

class Client {
 public:
  Client() = delete;
  explicit Client(int socket, const std::string &filename);
  ~Client();

  int RecvRequest();
  int RecvResponse();
  int SendRequest();
  int SendResponse();

  int GetClientSocket() const noexcept;
  int GetServerSocket() const noexcept;
  int GetStatus() const noexcept;
  void SetStatus(int status) noexcept;

 private:
  const static int kBufLen = 1024;

  void SetQueryMessageLength();

  int client_socket_{};
  std::string filename_ = "";

  int server_socket_ = -1;
  struct sockaddr_in server_sockaddr_;
  int status_ = 0;

  char buf_[kBufLen]{};
  int bytes_read_ = 0;
  long bytes_write_ = 0;

  std::string client_request_ = "";
  long client_message_length_ = 0;
  long sent_request_length_ = 0;

  std::string server_response_ = "";

  long unsigned query_message_length_ = 0LU;
};

#endif  // TCP_PROXY_SERVER_CLIENT_H_
