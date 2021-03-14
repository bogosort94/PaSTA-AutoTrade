#include "DataHandler/data_client.h"

#include <iostream>
#include "absl/flags/flag.h"
#include "absl/status/status.h"

ABSL_FLAG(bool, data_client_no_run, false,
          "The data client will stop running after subscribing to data "
          "supplier if this is set to true. Used for testing purpose only.");

namespace pasta {

const std::string DataClient::data_url = "wss://socket.polygon.io/stocks";

void DataClient::SetAuthentication(const std::string auth) { auth_ = auth; }
absl::Status DataClient::RegisterFunc(
    const std::string& name, std::function<void(std::string_view)> func) {
  if (reg_func_.count(name) > 0) {
    return absl::AlreadyExistsError("Function name <" + name +
                                    "> is already registered.");
  }
  reg_func_[name] = func;
  return absl::OkStatus();
}

absl::Status DataClient::UnregisterFunc(const std::string& name) {
  auto iter = reg_func_.find(name);
  if (iter == reg_func_.end()) {
    return absl::NotFoundError("Function name <" + name +
                               "> is not registered.");
  }
  reg_func_.erase(iter);
  return absl::OkStatus();
}

// static
context_ptr DataClient::OnTlsInit() {
  // establishes a SSL connection
  context_ptr ctx = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::sslv23);

  try {
    ctx->set_options(boost::asio::ssl::context::default_workarounds |
                     boost::asio::ssl::context::no_sslv2 |
                     boost::asio::ssl::context::no_sslv3 |
                     boost::asio::ssl::context::single_dh_use);
  } catch (std::exception& e) {
    std::cout << "Error in context pointer: " << e.what() << std::endl;
  }
  return ctx;
}

// TODO: Integrate with a logging util.
absl::Status DataClient::Run() {
  if (auth_.empty()) {
    std::cerr << "Authentication information not provided." << std::endl;
    return absl::UnauthenticatedError(
        "Authentication information not provided.");
  }
  try {
    // Set logging to be error-only
    c_.clear_access_channels(websocketpp::log::alevel::all);
    c_.set_error_channels(websocketpp::log::elevel::all);

    // Initialize ASIO
    c_.init_asio();
    c_.set_tls_init_handler(bind(&DataClient::OnTlsInit));

    // Register our message handler
    c_.set_message_handler(bind(&DataClient::OnMessage, this, &c_, ::_1, ::_2));

    websocketpp::lib::error_code ec;
    client::connection_ptr con = c_.get_connection(data_url.c_str(), ec);
    if (ec) {
      std::cerr << "Could not create connection because: " << ec.message()
                << std::endl;
      return absl::UnavailableError("Could not create connection because: " +
                                    ec.message());
    }

    // Note that connect here only requests a connection. No network messages
    // are exchanged until the event loop starts running in the next line.
    c_.connect(con);

    // Start the ASIO io_service run loop
    // this will cause a single connection to be made to the server. c.run()
    // will exit when this connection is closed.
    c_.run();
  } catch (websocketpp::exception const& e) {
    std::cout << e.what() << std::endl;
    return absl::InternalError(e.what());
  }

  return status_;
}

void DataClient::OnMessage(client* c, websocketpp::connection_hdl hdl,
                           client::message_ptr msg) {
  std::cout << "\t" << msg->get_payload() << std::endl;
  websocketpp::lib::error_code ec;
  switch (state_) {
    case INIT:
      if (msg->get_payload().find("Connected Successfully") !=
          std::string::npos) {
        state_ = CONNECTED;
        c->send(
            hdl,
            std::string("{\"action\":\"auth\",\"params\":\"" + auth_ + "\"}"),
            websocketpp::frame::opcode::text, ec);
        if (ec) {
          status_ =
              absl::AbortedError("Failed sending authentication message.");
          c->stop();
        }
      } else {
        status_ =
            absl::UnavailableError("Unexpected message: " + msg->get_payload());
        c->stop();
      }
      break;
    case CONNECTED:
      if (msg->get_payload().find("authenticated") != std::string::npos) {
        state_ = AUTHENTICATED;
        c->send(hdl,
                std::string("{\"action\":\"subscribe\",\"params\":\"A.*\"}"),
                websocketpp::frame::opcode::text, ec);
        if (ec) {
          status_ = absl::AbortedError("Failed subscribing to data supplier.");
          c->stop();
        }
      } else {
        status_ = absl::UnauthenticatedError("Unexpected message: " +
                                             msg->get_payload());
        c->stop();
      }
      break;
    case AUTHENTICATED:
      if (msg->get_payload().find("subscribed to") != std::string::npos) {
        state_ = SUBSCRIBED;
      } else {
        status_ =
            absl::AbortedError("Unexpected message: " + msg->get_payload());
        c->stop();
      }
      if (absl::GetFlag(FLAGS_data_client_no_run)) {
        c->stop();
      }
      break;
    case SUBSCRIBED:
      for (auto name_func : reg_func_) {
        name_func.second(msg->get_payload());
      }
      break;
    default:
      // case NUM_CLIENT_STATE
      status_ = absl::UnknownError("Unknown data client state.");
      c->stop();
  }
}

}  // namespace pasta
