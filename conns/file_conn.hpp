#pragma once

#include <exception>

#include "conn.hpp"

class AccessError : public std::exception {
public:
  explicit AccessError(const std::string &m) : m_{std::move(m)} {}
  const char *what() const noexcept override { return m_.c_str(); }

private:
  const std::string m_;
};

class FileConnection {
public:
  FileConnection(const std::string &access_token);
  FileConnection &UpdateAccessToken(const std::string &);
  std::string Create(const std::string &);
  void Update(const std::string &, const std::string &);

private:
  std::string access_token_;
  std::unique_ptr<Connection> conn_;
};
