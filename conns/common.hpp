#pragma once
#include <boost/asio/ssl.hpp>
void add_cert_auth_s(boost::asio::ssl::context &ctx,
                     boost::system::error_code &ec);
void add_cert_auth_s(boost::asio::ssl::context &ctx);
