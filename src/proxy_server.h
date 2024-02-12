#ifndef PROXY_SERVER_PSQL_PROXY_SERVER_H_
#define PROXY_SERVER_PSQL_PROXY_SERVER_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <list>
#include <sstream>

#include "client.h"
#include "utils.h"

namespace tcp {
class ProxyServer {
 public:
  ProxyServer() = delete;
  explicit ProxyServer(char *argv[]);
  ~ProxyServer();

  void Exec();

 private:
  enum { kRecvRequest, kSendRequest, kRecvResponse, kSendResponse };

  void SetFDs(fd_set *read_fds, fd_set *write_fds);
  void AcceptConnection();
  void ProcessConnections(fd_set *read_fds, fd_set *write_fds);

  int client_port_number_{};
  std::string filename_ = "";

  struct sockaddr_in client_sockaddr_ {};
  int client_listener_{};
  int max_fds_{};

  std::list<Client *> clients_;
};
}  // namespace tcp

#endif  // PROXY_SERVER_PSQL_PROXY_SERVER_H_
