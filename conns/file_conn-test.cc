#include <gtest/gtest.h>

#include "common-test.hpp"
#include "file_conn.hpp"

TEST(FileConnTest, Create) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Authorization("Bearer access-token"))
      .WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, ContentType(StartsWith("multipart/related;")))
      .WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do())
      .WillOnce(Return(Response{200, "{\"id\":\"dummy-id\"}"}));

  FileConnection file_conn("access-token");

  EXPECT_CALL(GetMockConnection(),
              Post("/upload/drive/v3/files?uploadType=multipart",
                   HasSubstr("dummy-value")))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_EQ(file_conn.Create("dummy-value"), "dummy-id");
}

TEST(FileConnTest, CreateFail) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Authorization(_)).WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, ContentType(_)).WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{401, "{}"}));

  FileConnection file_conn("access-token");

  EXPECT_CALL(GetMockConnection(), Post(_, _))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_THROW(file_conn.Create("dummy-value"), AccessError);
}

TEST(FileConnTest, Update) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Authorization("Bearer access-token"))
      .WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{200, ""}));

  FileConnection file_conn("access-token");

  EXPECT_CALL(
      GetMockConnection(),
      Patch("/upload/drive/v3/files/dummy-id?uploadType=media", "dummy-ip"))
      .WillOnce(Return(std::move(mock_req)));

  file_conn.Update("dummy-id", "dummy-ip");
}

TEST(FileConnTest, UpdateFail) {
  auto mock_req = std::make_unique<MockRequest>();
  EXPECT_CALL(*mock_req, Authorization(_)).WillOnce(ReturnRef(*mock_req));
  EXPECT_CALL(*mock_req, Do()).WillOnce(Return(Response{401, "{}"}));

  FileConnection file_conn("access-token");

  EXPECT_CALL(GetMockConnection(), Patch(_, _))
      .WillOnce(Return(std::move(mock_req)));

  EXPECT_THROW(file_conn.Update("", ""), AccessError);
}
