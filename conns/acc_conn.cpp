#include <memory>
#include <sstream>

#include <nlohmann/json.hpp>
#include <stdexcept>

#include "acc_conn.hpp"
#include "conn.hpp"

static const std::string host = "accounts.google.com";
static const std::string refresh_token_target = "/o/oauth2/token";

AccountConnection::AccountConnection(const std::string &client_id,
                                     const std::string &client_secret,
                                     const std::string &refresh_token)
    : client_id_{client_id}, client_secret_{client_secret},
      refresh_token_{refresh_token}, conn_{Connection::Https(host)} {}

std::string AccountConnection::GetAccessToken() {
  std::ostringstream oss;
  oss << "client_id=" << client_id_ << "&client_secret=" << client_secret_
      << "&refresh_token=" << refresh_token_ << "&grant_type=refresh_token";
  std::string body = oss.str();

  std::unique_ptr<Request> req = conn_->Post(refresh_token_target, body);
  req->ContentType("application/x-www-form-urlencoded");

  Response res = req->Do();

  if (not res.IsOk()) {
    throw std::runtime_error{res.body};
  }

  return nlohmann::json::parse(res.body)["access_token"];
}
