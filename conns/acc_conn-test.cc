#include <gtest/gtest.h>

#include "acc_conn.hpp"
#include "common-test.hpp"

TEST(AccConnTest, GetAccessToken) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, ContentType("application/x-www-form-urlencoded"))
      .WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do())
      .WillOnce(Return(Response{200, "{\"access_token\":\"dummy-token\"}"}));

  AccountConnection acc_conn("client-id", "client-secret", "refresh-token");

  EXPECT_CALL(GetMockConnection(),
              Post(_, "client_id=client-id&client_secret=client-secret&refresh_"
                      "token=refresh-token&grant_type=refresh_token"))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_EQ(acc_conn.GetAccessToken(), "dummy-token");
}

TEST(AccConnTest, GetAccessTokenFail) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, ContentType("application/x-www-form-urlencoded"))
      .WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{401, "{}"}));

  AccountConnection acc_conn("client-id", "client-secret", "refresh-token");

  EXPECT_CALL(GetMockConnection(),
              Post(_, "client_id=client-id&client_secret=client-secret&refresh_"
                      "token=refresh-token&grant_type=refresh_token"))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_THROW(acc_conn.GetAccessToken(), std::runtime_error);
}
