#include <gtest/gtest.h>

#include "common-test.hpp"
#include "ip_conn.hpp"

TEST(IpConnTest, GetRightOutput) {
  IpConnection ip_conn;

  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Agent("curl")).WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{200, "output"}));

  EXPECT_CALL(GetMockConnection(), Get("/"))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_EQ(ip_conn.GetIp(), "output");
}

TEST(IpConnTest, GetWrongOutput) {
  IpConnection ip_conn;

  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Agent("curl")).WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{401, "output"}));

  EXPECT_CALL(GetMockConnection(), Get("/"))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_THROW(ip_conn.GetIp(), std::runtime_error);
  ;
}
