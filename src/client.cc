#include "client.h"

Client::~Client() {
  close(client_socket_);
  close(server_socket_);
}

Client::Client(int socket_num, const std::string &filename)
    : client_socket_(socket_num), filename_(filename) {
  utils::CheckResult((server_socket_ = socket(AF_INET, SOCK_STREAM, 0)),
                     "socket");

  server_sockaddr_.sin_family = AF_INET;
  server_sockaddr_.sin_port = htons(5432);
  server_sockaddr_.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  utils::CheckResult(
      connect(server_socket_, (struct sockaddr *)&server_sockaddr_,
              sizeof(server_sockaddr_)),
      "connect");
}

int Client::RecvRequest() {
  int result = 1;

  if ((bytes_read_ = recv(client_socket_, buf_, kBufLen, 0)) < 0) {
    result = -1;
  } else {
    client_request_.append(buf_, bytes_read_);
    if (buf_[0] == 'Q') {
      SetQueryMessageLength();
    }

    if ((client_request_.size() < kBufLen) ||
        ((client_request_[0] == 'Q') &&
         (client_request_.size() == query_message_length_))) {
      client_message_length_ = client_request_.size();

      if ((client_request_.length() > 0) && (client_request_[0] == 'Q')) {
        query_message_length_ = 0;
        utils::WriteLog(filename_,
                        client_request_.substr(5, client_request_.size() - 6));
      }

      result = 0;
    }
  }

  return result;
}

int Client::RecvResponse() {
  int result = 0;

  if ((bytes_read_ = recv(server_socket_, buf_, kBufLen, 0)) < 0) {
    result = -1;
  } else {
    server_response_.append(buf_, bytes_read_);
  }

  return result;
}

int Client::SendRequest() {
  int result = 1;

  if ((bytes_write_ =
           send(server_socket_, client_request_.c_str() + sent_request_length_,
                client_message_length_, 0)) >= 0) {
    sent_request_length_ += bytes_write_;
    client_message_length_ -= bytes_write_;

    if (client_message_length_ == 0) {
      sent_request_length_ = 0;
      client_request_.clear();
      result = 0;
    }
  } else {
    result = -1;
  }

  return result;
}

int Client::SendResponse() {
  int result = send(client_socket_, server_response_.c_str(),
                    server_response_.size(), 0);

  if (result >= 0) {
    server_response_.clear();
    result = 0;
  }

  return result;
}

int Client::GetClientSocket() const noexcept { return client_socket_; }

int Client::GetServerSocket() const noexcept { return server_socket_; }

int Client::GetStatus() const noexcept { return status_; }

void Client::SetStatus(int status) noexcept { status_ = status; }

void Client::SetQueryMessageLength() {
  if ((client_request_.length() >= 5) && (client_request_[0] == 'Q')) {
    query_message_length_ =
        (long unsigned)((unsigned char)(client_request_[1]) << 24 |
                        (unsigned char)(client_request_[2]) << 16 |
                        (unsigned char)(client_request_[3]) << 8 |
                        (unsigned char)(client_request_[4])) +
        1LU;
  }
}
