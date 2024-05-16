#pragma once

#include <memory>
#include <string>

class Response {
public:
  bool IsOk() const noexcept { return status_code == 200; }

  std::size_t status_code;
  std::string body;
};

class Request {
public:
  virtual ~Request() = default;

  virtual Request &Agent(const std::string &) = 0;
  virtual Request &Authorization(const std::string &) = 0;
  virtual Request &ContentType(const std::string &) = 0;
  virtual Response Do() = 0;
};

class Connection {
public:
  virtual ~Connection() = default;
  Connection(const Connection &) = delete;
  Connection &operator=(const Connection &) = delete;
  Connection(Connection &&) = default;
  Connection &operator=(Connection &&) = default;

  virtual std::unique_ptr<Request> Post(const std::string &target,
                                        const std::string &body) = 0;
  virtual std::unique_ptr<Request> Patch(const std::string &target,
                                         const std::string &body) = 0;
  virtual std::unique_ptr<Request> Get(const std::string &target) = 0;

  static std::unique_ptr<Connection> Https(const std::string &host);

protected:
  explicit Connection() = default;
};
