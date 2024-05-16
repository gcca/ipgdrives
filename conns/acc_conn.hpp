#pragma once

#include "conn.hpp"

class AccountConnection {
public:
  AccountConnection(const std::string &client_id,
                    const std::string &client_secret,
                    const std::string &refresh_token);

  std::string GetAccessToken();

private:
  const std::string client_id_;
  const std::string client_secret_;
  const std::string refresh_token_;
  std::unique_ptr<Connection> conn_;
};
