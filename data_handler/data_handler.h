#ifndef PASTA_DATA_HANDLER_DATA_HANDLER_H_
#define PASTA_DATA_HANDLER_DATA_HANDLER_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/inlined_vector.h"
#include "absl/status/status.h"
#include "data_handler/agg_data.h"
#include "data_handler/data_client.h"
#include "proto/data.pb.h"

#include <string_view>

namespace pasta {

enum DataStoreIndex {
  ONE_SEC,
  TEN_SEC,
  ONE_MINUTE,
  FIVE_MINUTE,
  NUM_DATA_STORE,
};

class DataHandler {
 public:
  DataHandler(DataClient* dc);

  void Init();

  void ProcessMessage(const std::string& msg);

  const AggDataStore::AggDataQueue& GetData(DataStoreIndex index,
                                            const std::string& ticker);

  absl::Status RegisterCallback(const std::string& name,
                                std::function<void(std::string)> cb);
  absl::Status UnregisterCallback(const std::string& name);

 private:
  void AddData(const AggregateDataProto& proto);

  DataClient* dc_;

  absl::InlinedVector<AggDataStore, NUM_DATA_STORE> agg_data_ = {
      AggDataStore(1), AggDataStore(10), AggDataStore(60), AggDataStore(300)};

  // Methods to call upon new data.
  absl::flat_hash_map<std::string, std::function<void(std::string)>>
      strategy_cb_;
};

}  // namespace pasta

#endif  // PASTA_DATA_HANDLER_DATA_HANDLER_H_
