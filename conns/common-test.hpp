#pragma once

#include "gmock/gmock.h"

#include "conn.hpp"

using ::testing::Return, ::testing::ReturnRef, ::testing::_,
    ::testing::HasSubstr, ::testing::StartsWith;

class MockRequest : public Request {
public:
  MOCK_METHOD(Request &, Agent, (const std::string &), (final));
  MOCK_METHOD(Request &, Authorization, (const std::string &), (final));
  MOCK_METHOD(Request &, ContentType, (const std::string &), (final));
  MOCK_METHOD(Response, Do, (), (final));
};

class MockConnection : public Connection {
public:
  MOCK_METHOD(std::unique_ptr<Request>, Post,
              (const std::string &, const std::string &), (final));
  MOCK_METHOD(std::unique_ptr<Request>, Patch,
              (const std::string &, const std::string &), (final));
  MOCK_METHOD(std::unique_ptr<Request>, Get, (const std::string &), (final));
};

[[nodiscard]] MockConnection &GetMockConnection();
