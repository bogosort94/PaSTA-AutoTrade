#ifndef AGG_DATA_H_
#define AGG_DATA_H_

#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "proto/data.pb.h"

#include <deque>
#include <string>

namespace pasta {

class AggregateData {
 public:
  AggregateData();
  AggregateData(const AggregateDataProto& agg_data);

  bool operator==(const AggregateData& agg_data) const;

  void UpdateWith(const AggregateDataProto& agg_data);

  std::string ticker_;
  int64_t vol_;
  int64_t acc_vol_;
  double day_open_;
  double vwap_;
  double open_;
  double close_;
  double high_;
  double low_;
  int64_t start_;
  int64_t end_;
};

class AggDataStore {
 public:
  AggDataStore();
  AggDataStore(int64_t window_size);

  typedef std::deque<AggregateData> AggDataQueue;
  const AggDataQueue& GetData(const std::string& ticker);

  // Add new aggregate data to the data store.
  // Returns true if an aggregate window is closed.
  // Note that when this method returns true, there can be at most two closing
  // aggregate window (previous and new). However, if a new window is closed
  // right upon creation, it does not make the return value true when the next
  // aggregate window is created.
  bool AddData(const AggregateDataProto& data_proto);

  void Clear();

 private:
  // Add new aggregate data to a specific queue.
  bool AddDataHelper(const AggregateDataProto& data_proto, AggDataQueue* data);

  // Determine if the new data still belongs to the previous aggregate window.
  bool IsNewAggregate(const AggregateDataProto& data_proto,
                      const AggregateData& prev_agg) const;

  // Given the start ts of a piece of data, calculate the start ts of the
  // aggregate window it belongs to.
  int64_t AggWindowStart(int64_t start) const;

  // Returns true if an aggregate window at the head of queue is closed.
  bool IsClosed(const AggregateData& data) const;

  // The size of the aggregate window (seconds).
  int64_t window_size_;

  // The queue storing aggregate data. New data will be added to head of queue.
  // Old data will be popped from back of queue.
  absl::flat_hash_map<std::string, AggDataQueue> data_;
};

}  // namespace pasta

extern absl::Flag<int64_t> FLAGS_agg_data_store_size;

#endif  // AGG_DATA_H_
