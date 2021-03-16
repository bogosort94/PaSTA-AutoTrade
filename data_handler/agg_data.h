#ifndef AGG_DATA_H_
#define AGG_DATA_H_

#include "proto/data.pb.h"

#include <string>

namespace pasta {

class AggregateData {
 public:
  AggregateData();
  AggregateData(const AggregateDataProto& agg_data);

  bool operator==(const AggregateData& agg_data) const;

  void UpdateWith(const AggregateDataProto& agg_data);

 private:
  std::string ticker_;
  int vol_;
  int acc_vol_;
  double day_open_;
  double vwap_;
  double open_;
  double close_;
  double high_;
  double low_;
  int start_;
  int end_;
};

}  // namespace pasta

#endif  // AGG_DATA_H_
