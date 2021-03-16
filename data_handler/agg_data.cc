#include "data_handler/agg_data.h"

#include "glog/logging.h"
#include "proto/data.pb.h"

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

}  // namespace pasta
