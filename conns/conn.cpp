#include "conn.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <openssl/tls1.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

#include "common.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace {

static constexpr char port_[] = "443";
static constexpr int version_ = 11;

class BaredRequest : public Request {
public:
  BaredRequest(const std::string &host, const std::string &target,
               http::verb verb, beast::ssl_stream<beast::tcp_stream> &stream)
      : req_{verb, target.c_str(), version_}, stream_{stream} {
    req_.set(http::field::host, host);
  }

  Request &Agent(const std::string &agent) final {
    req_.set(http::field::user_agent, agent);
    return *this;
  }

  Request &Authorization(const std::string &authorization) final {
    req_.set(http::field::authorization, authorization);
    return *this;
  }

  Request &ContentType(const std::string &content_type) final {
    req_.set(http::field::content_type, content_type);
    return *this;
  }

  Response Do() final {
    http::write(stream_, req_);

    beast::flat_buffer buffer;

    http::response<http::dynamic_body> res;
    http::read(stream_, buffer, res);

    return Response{res.result_int(),
                    boost::beast::buffers_to_string(res.body().data())};
  }

  http::request<http::string_body> req_;
  beast::ssl_stream<beast::tcp_stream> &stream_;
};

class LoadedRequest : public BaredRequest {
public:
  LoadedRequest(const std::string &host, const std::string &target,
                http::verb verb, beast::ssl_stream<beast::tcp_stream> &stream,
                const std::string &body)
      : BaredRequest(host, target, verb, stream) {
    req_.body() = body;
    req_.prepare_payload();
  }
};

class HttpsConnection : public Connection {
public:
  HttpsConnection(const std::string &host)
      : host_{host}, ctx_{ssl::context::tlsv12_client}, stream_{ioc_, ctx_} {
    add_cert_auth_s(ctx_);
    ctx_.set_verify_mode(ssl::verify_peer);

    tcp::resolver resolver(ioc_);

    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host_.c_str())) {

      beast::error_code ec{static_cast<int>(::ERR_get_error()),
                           net::error::get_ssl_category()};
      throw beast::system_error{ec};
    }

    auto const results = resolver.resolve(host, port_);

    beast::get_lowest_layer(stream_).connect(results);

    stream_.handshake(ssl::stream_base::client);
  }

  ~HttpsConnection() {
    beast::error_code ec;
    stream_.shutdown(ec);
    if (ec && ec != beast::errc::not_connected) {
      std::cerr << "Error: " << ec << std::endl;
    }
  }

  std::unique_ptr<Request> Post(const std::string &target,
                                const std::string &body) final {
    return std::make_unique<LoadedRequest>(host_, target, http::verb::post,
                                           stream_, body);
  }

  std::unique_ptr<Request> Patch(const std::string &target,
                                 const std::string &body) final {
    return std::make_unique<LoadedRequest>(host_, target, http::verb::patch,
                                           stream_, body);
  }

  std::unique_ptr<Request> Get(const std::string &target) final {
    return std::make_unique<BaredRequest>(host_, target, http::verb::get,
                                          stream_);
  }

private:
  const std::string host_;
  net::io_context ioc_;
  ssl::context ctx_;
  beast::ssl_stream<beast::tcp_stream> stream_;
};

} // namespace

std::unique_ptr<Connection> Connection::Https(const std::string &host) {
  return std::make_unique<HttpsConnection>(host);
}
