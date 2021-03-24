#include "data_handler/agg_data.h"

#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "glog/logging.h"
#include "proto/data.pb.h"

ABSL_FLAG(
    int64_t, agg_data_store_size, 30,
    "The amount of aggregate windows that each AggDataStore keeps track of.");

namespace pasta {

//=============== AggregateData ===============

AggregateData::AggregateData()
    : ticker_(""),
      vol_(0),
      acc_vol_(0),
      day_open_(0.),
      vwap_(0.),
      open_(0.),
      close_(0.),
      high_(0.),
      low_(0.),
      start_(0),
      end_(0) {}

AggregateData::AggregateData(const AggregateDataProto& agg_data)
    : ticker_(agg_data.sym()),
      vol_(agg_data.v()),
      acc_vol_(agg_data.av()),
      day_open_(agg_data.op()),
      vwap_(agg_data.vw()),
      open_(agg_data.o()),
      close_(agg_data.c()),
      high_(agg_data.h()),
      low_(agg_data.l()),
      start_(agg_data.s()),
      end_(agg_data.e()) {}

bool AggregateData::operator==(const AggregateData& agg_data) const {
  return ticker_ == agg_data.ticker_ && vol_ == agg_data.vol_ &&
         acc_vol_ == agg_data.acc_vol_ && day_open_ == agg_data.day_open_ &&
         vwap_ == agg_data.vwap_ && open_ == agg_data.open_ &&
         close_ == agg_data.close_ && high_ == agg_data.high_ &&
         low_ == agg_data.low_ && start_ == agg_data.start_ &&
         end_ == agg_data.end_;
}

void AggregateData::UpdateWith(const AggregateDataProto& agg_data) {
  CHECK(ticker_ == agg_data.sym());
  vol_ += agg_data.v();
  acc_vol_ = agg_data.av();
  vwap_ = agg_data.vw();
  close_ = agg_data.c();
  high_ = std::max(high_, agg_data.h());
  low_ = std::min(low_, agg_data.l());
  end_ = agg_data.e();
}

//=============== AggDataStore ===============

AggDataStore::AggDataStore() : window_size_(1) {}
AggDataStore::AggDataStore(int64_t window_size) : window_size_(window_size) {}

const AggDataStore::AggDataQueue& AggDataStore::GetData(
    const std::string& ticker) {
  return data_[ticker];
}

bool AggDataStore::AddData(const AggregateDataProto& data_proto) {
  CHECK(data_proto.ev() == "A");
  std::string ticker = data_proto.sym();

  auto& data = data_[ticker];
  if (data.empty() || IsNewAggregate(data_proto, data.front())) {
    // Add a new aggregate window. Align timestamp.
    bool old_window_not_closed = !data.empty() && !IsClosed(data.front());
    data.push_front(AggregateData(data_proto));
    data.front().start_ = AggWindowStart(data.front().start_);
    if (data.size() > absl::GetFlag(FLAGS_agg_data_store_size)) {
      data.pop_back();
    }
    return IsClosed(data.front()) || old_window_not_closed;
  } else {
    // Update existing aggregate window with new data.
    data.front().UpdateWith(data_proto);
    return IsClosed(data.front());
  }
}

void AggDataStore::Clear() { data_.clear(); }

bool AggDataStore::IsNewAggregate(const AggregateDataProto& data_proto,
                                  const AggregateData& prev_agg) const {
  return data_proto.s() - prev_agg.start_ >=
         window_size_ * NUM_MILLIS_PER_SECOND;
}

int64_t AggDataStore::AggWindowStart(int64_t start) const {
  return start - start % (window_size_ * NUM_MILLIS_PER_SECOND);
}

bool AggDataStore::IsClosed(const AggregateData& data) const {
  CHECK(data.end_ - data.start_ <= window_size_ * NUM_MILLIS_PER_SECOND);
  return data.end_ - data.start_ == window_size_ * NUM_MILLIS_PER_SECOND;
}

}  // namespace pasta
