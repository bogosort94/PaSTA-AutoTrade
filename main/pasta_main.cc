#include "absl/status/status.h"
#include "data_handler/data_client.h"
#include "data_handler/data_handler.h"
#include "glog/logging.h"
//#include "strategy/chase_momentum_strategy.h"

int main(int argc, char* argv[]) {
  absl::Status s;
  LOG(INFO) << "PaSTA Auto Trader starts running.";
  pasta::DataClient dc;
  dc.SetAuthentication(dc.GetCredential());
  LOG(INFO) << "Data client set authentication done.";
  // pasta::DataHandler dh = pasta::DataHandler(&dc);
  // dh.Init();
  // LOG(INFO) << "Data handler initialization done.";

  // pasta::ChaseMomentumStrategy c_m_s = pasta::ChaseMomentumStrategy(&dh);
  // s = c_m_s.Init();
  // LOG(INFO) << "Chase Momentum Strategy initialization done.";
  // if (!s.ok()) {
  //   LOG(FATAL) << "Chase Momentum Strategy initialization failure.";
  // }

  s = dc.Run();
  if (s.ok()) {
    LOG(WARNING) << "Data client stopped with OK status.";
  } else {
    LOG(ERROR) << "Data client failure: " << s.ToString();
  }
  return 0;
}
