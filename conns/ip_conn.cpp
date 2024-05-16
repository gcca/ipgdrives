#include <iostream>

#include "ip_conn.hpp"

static const std::string host = "ip.me";

IpConnection::IpConnection() : conn_{Connection::Https(host)} {}

std::string IpConnection::GetIp() {
  std::unique_ptr<Request> req = conn_->Get("/");
  Response res = req->Agent("curl").Do();

  if (not res.IsOk()) {
    std::cerr << res.body << std::endl;
    throw std::runtime_error{"no ip"};
  }

  return res.body;
}
