#include <unordered_map>

#include "common-test.hpp"

std::unordered_map<std::string, MockConnection *> map;

std::unique_ptr<Connection> Connection::Https(const std::string &) {
  auto conn = std::make_unique<MockConnection>();
  map[testing::UnitTest::GetInstance()->current_test_info()->name()] =
      conn.get();
  return conn;
}

[[nodiscard]] MockConnection &GetMockConnection() {
  return *map[testing::UnitTest::GetInstance()->current_test_info()->name()];
}
