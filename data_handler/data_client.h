#ifndef PASTA_DATA_HANDLER_DATA_CLIENT_H_
#define PASTA_DATA_HANDLER_DATA_CLIENT_H_

#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/status/status.h"

#include <string_view>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/frame.hpp>


using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

namespace pasta {

typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

class DataClient {
 public:
  DataClient() : status_(absl::OkStatus()), state_(INIT) {}

  void SetAuthentication(const std::string auth);
  absl::Status RegisterFunc(const std::string& name,
                            std::function<void(std::string_view)> func);
  absl::Status UnregisterFunc(const std::string& name);
  absl::Status Run();

 private:
  enum ClientState {
    INIT = 0,
    CONNECTED = 1,
    AUTHENTICATED = 2,
    SUBSCRIBED = 3,
    NUM_CLIENT_STATE = 4,
  };

  static context_ptr OnTlsInit();
  void OnMessage(client* c, websocketpp::connection_hdl hdl,
                 client::message_ptr msg);

  // Data supplier url.
  static const std::string data_url;

  // WebSocket client.
  client c_;

  // Client status.
  absl::Status status_;

  // Authentication.
  std::string auth_;

  // Data client state.
  ClientState state_;

  // Functions to be called upon message.
  absl::flat_hash_map<std::string, std::function<void(std::string_view)>>
      reg_func_;
};

}  // namespace pasta

// For testing purpose only.
extern absl::Flag<bool> FLAGS_data_client_no_run;

#endif  // PASTA_DATA_HANDLER_DATA_CLIENT_H_
