#include "data_handler/data_handler.h"

#include "absl/status/status.h"
#include "data_handler/agg_data.h"
#include "data_handler/data_client.h"
#include "glog/logging.h"
#include "google/protobuf/util/json_util.h"
#include "proto/data.pb.h"

#include <string_view>

namespace pasta {

DataHandler::DataHandler(DataClient* dc) : dc_(dc) {}

void DataHandler::Init() {
  absl::Status s = dc_->RegisterFunc(
      "data_handler_process_message",
      std::bind(&DataHandler::ProcessMessage, this, std::placeholders::_1));
}

const AggDataStore::AggDataQueue& DataHandler::GetData(
    DataStoreIndex index, const std::string& ticker) {
  return agg_data_[index].GetData(ticker);
}

void DataHandler::ProcessMessage(const std::string& msg) {
  AggregateDataResponseProto proto;
  DLOG(INFO) << "Processing message " << msg;
  auto s =
      google::protobuf::util::JsonStringToMessage("{aggs:" + msg + "}", &proto);
  CHECK(s.ok());
  CHECK(proto.aggs_size() > 0);
  for (int i = 0; i < proto.aggs_size(); ++i) {
    AddData(proto.aggs(i));
  }
}

void DataHandler::AddData(const AggregateDataProto& proto) {
  for (auto& data : agg_data_) {
    data.AddData(proto);
  }
  for (auto name_cb : strategy_cb_) {
    name_cb.second(proto.sym());
  }
}

absl::Status DataHandler::RegisterCallback(
    const std::string& name, std::function<void(std::string)> cb) {
  if (strategy_cb_.count(name) > 0) {
    return absl::AlreadyExistsError("Callback name <" + name +
                                    "> is already registered.");
  }
  strategy_cb_[name] = cb;
  return absl::OkStatus();
}

absl::Status DataHandler::UnregisterCallback(const std::string& name) {
  auto iter = strategy_cb_.find(name);
  if (iter == strategy_cb_.end()) {
    return absl::NotFoundError("Callback name <" + name +
                               "> is not registered.");
  }
  strategy_cb_.erase(iter);
  return absl::OkStatus();
}

}  // namespace pasta
