#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include "conn.hpp"
#include "file_conn.hpp"

static const std::string host = "www.googleapis.com";
static const std::string files_base_target = "/drive/v3/files";
static const std::string files_update_target = "/upload" + files_base_target;
static const std::string files_create_target =
    files_update_target + "?uploadType=multipart";

FileConnection::FileConnection(const std::string &access_token)
    : conn_{Connection::Https(host)}, access_token_{access_token} {}

FileConnection &
FileConnection::UpdateAccessToken(const std::string &access_token) {
  access_token_ = access_token;
  return *this;
}

static inline std::string MakeSlug(std::size_t size) {
  std::string s(size, '\0');
  std::generate_n(s.begin(), size, []() {
    static constexpr char charset[] = "0123456789"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static constexpr std::size_t mz = sizeof(charset) - 1;
    return charset[std::rand() % mz];
  });
  return s;
}

std::string FileConnection::Create(const std::string &ip) {
  const std::string slug = MakeSlug(12);
  std::ostringstream oss;
  oss << "--" << slug
      << "\r\nContent-Type: application/json; chatset=UTF-8\r\n\r\n"
         "{\"name\":\"current_ip.txt\"}\r\n\r\n--"
      << slug << "\r\nContent-Type: text/plain\r\n\r\n"
      << ip << "\r\n--" << slug << "--";
  const std::string body = oss.str();

  std::unique_ptr<Request> req = conn_->Post(files_create_target, body);
  Response res = req->Authorization("Bearer " + access_token_)
                     .ContentType("multipart/related; boundary=" + slug)
                     .Do();

  if (not res.IsOk()) {
    throw AccessError{res.body};
  }

  return nlohmann::json::parse(res.body)["id"];
}

void FileConnection::Update(const std::string &id, const std::string &ip) {
  std::unique_ptr<Request> req =
      conn_->Patch(files_update_target + "/" + id + "?uploadType=media", ip);
  Response res = req->Authorization("Bearer " + access_token_).Do();

  if (not res.IsOk()) {
    throw AccessError{res.body};
  }
}
