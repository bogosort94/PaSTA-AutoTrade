#ifndef PASTA_STRATEGY_STRATEGY_H_
#define PASTA_STRATEGY_STRATEGY_H_

#include "absl/status/status.h"
#include "data_handler/data_handler.h"

namespace pasta {

class Strategy {
 public:
  Strategy(DataHandler* dh) : dh_(dh) {}
  virtual absl::Status Init() = 0;

 protected:
  DataHandler* dh_;
  const static absl::TimeZone nyc_tz;
};

}  // namespace pasta

#endif  // PASTA_STRATEGY_STRATEGY_H_
