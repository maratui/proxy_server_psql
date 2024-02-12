#include "proxy_server.h"

using namespace tcp;

ProxyServer::~ProxyServer() {
  for (auto clients_it = clients_.begin(); clients_it != clients_.end();
       ++clients_it) {
    close((*clients_it)->GetClientSocket());
    close((*clients_it)->GetServerSocket());
  }
}

ProxyServer::ProxyServer(char *argv[]) : filename_(argv[2]) {
  std::istringstream str_iss(argv[1]);

  utils::CheckResult((client_listener_ = socket(AF_INET, SOCK_STREAM, 0)),
                     "socket");
  utils::CheckResult(fcntl(client_listener_, F_SETFL, O_NONBLOCK), "fcntl");
  utils::CheckResult(setsockopt(client_listener_, SOL_SOCKET, SO_REUSEADDR,
                                &kOptVal, sizeof(kOptVal)),
                     "setsockopt");

  str_iss >> client_port_number_;
  client_sockaddr_.sin_family = AF_INET;
  client_sockaddr_.sin_port = htons(client_port_number_);
  client_sockaddr_.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  utils::CheckResult(
      bind(client_listener_, (struct sockaddr *)&client_sockaddr_,
           sizeof(client_sockaddr_)),
      "bind");

  utils::CheckResult(listen(client_listener_, SOMAXCONN), "listen");
}

void ProxyServer::Exec() {
  fd_set read_fds;
  fd_set write_fds;

  while (true) {
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    SetFDs(&read_fds, &write_fds);
    utils::CheckResult(select(max_fds_ + 1, &read_fds, &write_fds, NULL, NULL),
                       "select");
    if (FD_ISSET(client_listener_, &read_fds)) {
      AcceptConnection();
    }
    ProcessConnections(&read_fds, &write_fds);
  }
}

void ProxyServer::SetFDs(fd_set *read_fds, fd_set *write_fds) {
  std::list<int> sockets;

  sockets.push_back(client_listener_);
  FD_SET(client_listener_, read_fds);

  for (auto clients_it = clients_.begin(); clients_it != clients_.end();
       ++clients_it) {
    int status = (*clients_it)->GetStatus();
    int socket_fd = 0;

    if (status == kRecvRequest) {
      socket_fd = (*clients_it)->GetClientSocket();
      FD_SET(socket_fd, read_fds);
    } else if (status == kRecvResponse) {
      socket_fd = (*clients_it)->GetServerSocket();
      FD_SET(socket_fd, read_fds);
    } else if (status == kSendResponse) {
      socket_fd = (*clients_it)->GetClientSocket();
      FD_SET(socket_fd, write_fds);
    } else if (status == kSendRequest) {
      socket_fd = (*clients_it)->GetServerSocket();
      FD_SET(socket_fd, write_fds);
    }
    sockets.push_back(socket_fd);
  }

  max_fds_ = *std::max_element(sockets.begin(), sockets.end());
}

void ProxyServer::AcceptConnection() {
  int socket{};

  utils::CheckResult((socket = accept(client_listener_, NULL, NULL)), "accept");
  utils::CheckResult(fcntl(socket, F_SETFL, O_NONBLOCK), "fcntl");
  utils::CheckResult(setsockopt(client_listener_, SOL_SOCKET, SO_REUSEADDR,
                                &kOptVal, sizeof(kOptVal)),
                     "setsockopt");

  clients_.push_back(new Client(socket, filename_));
}

void ProxyServer::ProcessConnections(fd_set *read_fds, fd_set *write_fds) {
  for (auto clients_it = clients_.begin(); clients_it != clients_.end();) {
    int client_socket = (*clients_it)->GetClientSocket();
    int server_socket = (*clients_it)->GetServerSocket();
    int result = 0;
    if (FD_ISSET(client_socket, read_fds)) {
      if ((result = (*clients_it)->RecvRequest()) == 0) {
        (*clients_it)->SetStatus(kSendRequest);
      }
    } else if (FD_ISSET(server_socket, read_fds)) {
      if ((result = (*clients_it)->RecvResponse()) == 0) {
        (*clients_it)->SetStatus(kSendResponse);
      }
    }
    if (FD_ISSET(server_socket, write_fds)) {
      if ((result = (*clients_it)->SendRequest()) == 0) {
        (*clients_it)->SetStatus(kRecvResponse);
      }
    } else if (FD_ISSET(client_socket, write_fds)) {
      if ((result = (*clients_it)->SendResponse()) == 0) {
        (*clients_it)->SetStatus(kRecvRequest);
      }
    }
    if (result < 0) {
      delete (*clients_it);
      clients_it = clients_.erase(clients_it);
      utils::OutMessage("connection closed");
    } else {
      ++clients_it;
    }
  }
}
