#include <iostream>

#include "proxy_server.h"

int main(int argc, char *argv[]) {
  if (argc == 3) {
    tcp::ProxyServer server = tcp::ProxyServer(argv);

    server.Exec();
  } else {
    std::cout << "TCP прокси-сервер для СУБД Postgresql должен получить из "
                 "командной строки 2 параметра:\n 1) "
                 "Номер порта клиента\n 2) Путь для логирования всех SQL "
                 "запросов, проходящих через прокси-сервер\n";
  }

  return 0;
}
