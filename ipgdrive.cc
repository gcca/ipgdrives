#include <chrono>
#include <csignal>
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include <boost/program_options.hpp>
#include <sys/signal.h>

#include "conns/acc_conn.hpp"
#include "conns/file_conn.hpp"
#include "conns/ip_conn.hpp"

namespace po = boost::program_options;
volatile bool exit_state = false;

void exit_handler(int) { exit_state = true; }

class IpGdrive {
public:
  IpGdrive(const std::string &client_id, const std::string &client_secret,
           const std::string &refresh_token)
      : account_connection_{client_id, client_secret, refresh_token},
        file_connection_{account_connection_.GetAccessToken()} {}

  class Handler {
  public:
    explicit Handler(AccountConnection &account_connection,
                     FileConnection &file_connection,
                     IpConnection &ip_connection, const std::string &&id,
                     const std::string &&ip)
        : account_connection_{account_connection},
          file_connection_{file_connection}, ip_connection_{ip_connection},
          id_{std::forward<const std::string>(id)},
          ip_{std::forward<const std::string>(ip)} {}

    void KeepUpdating() {
      for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        const std::string curr_ip = ip_connection_.GetIp();

        if (ip_ != curr_ip) {
          ip_ = curr_ip;
          UpdateIp();
        }

        if (exit_state) {
          break;
        }
      }
    }

    void UpdateIp() {
      try {
        file_connection_.Update(id_, ip_);
      } catch (const AccessError &) {
        file_connection_.UpdateAccessToken(account_connection_.GetAccessToken())
            .Update(id_, ip_);
      }
    }

  private:
    const std::string id_;
    std::string ip_;
    AccountConnection &account_connection_;
    FileConnection &file_connection_;
    IpConnection &ip_connection_;
  };

  Handler CreateFile() {
    std::string ip = ip_connection_.GetIp();
    try {
      const std::string id = file_connection_.Create(ip);
      return Handler(account_connection_, file_connection_, ip_connection_,
                     std::move(id), std::move(ip));
    } catch (const AccessError &) {
      const std::string id =
          file_connection_
              .UpdateAccessToken(account_connection_.GetAccessToken())
              .Create(ip);
      return Handler(account_connection_, file_connection_, ip_connection_,
                     std::move(id), std::move(ip));
    }
  }

private:
  AccountConnection account_connection_;
  FileConnection file_connection_;
  IpConnection ip_connection_;
};

int main(int argc, char *argv[]) {
  std::string client_id, client_secret, refresh_token;

  try {
    po::options_description desc("IpGdrive");
    desc.add_options()("client_id", po::value<std::string>(), "Client Id")(
        "client_secret", po::value<std::string>(), "Client Secret")(
        "refresh_token", po::value<std::string>(), "Refresh Token");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    client_id = vm["client_id"].as<std::string>();
    client_secret = vm["client_secret"].as<std::string>();
    refresh_token = vm["refresh_token"].as<std::string>();
  } catch (std::exception &e) {
    std::cerr << "A Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  struct sigaction sighandler;
  sighandler.sa_handler = exit_handler;
  sigemptyset(&sighandler.sa_mask);
  sighandler.sa_flags = 0;
  sigaction(SIGINT, &sighandler, nullptr);

  IpGdrive ipgdrive(client_id, client_secret, refresh_token);
  IpGdrive::Handler handler = ipgdrive.CreateFile();
  handler.KeepUpdating();

  return EXIT_SUCCESS;
}
