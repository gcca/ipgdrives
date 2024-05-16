#pragma once

#include "conn.hpp"

class IpConnection {
public:
  IpConnection();
  std::string GetIp();

private:
  std::unique_ptr<Connection> conn_;
};
