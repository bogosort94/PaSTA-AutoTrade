#include "data_handler/data_client.h"

#include "absl/flags/flag.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

namespace pasta {

namespace {

class DataClientTest : public ::testing::Test {};

TEST_F(DataClientTest, ConnectAndSubscribe) {
  absl::SetFlag(&FLAGS_data_client_no_run, true);
  DataClient dc;
  dc.SetAuthentication(dc.GetCredential());
  absl::Status s = dc.Run();
  EXPECT_EQ(s, absl::OkStatus());
}

TEST_F(DataClientTest, AuthenticationFailure) {
  DataClient dc;
  dc.SetAuthentication("INVALIDAUTHENTICATIONINFO");
  absl::Status s = dc.Run();
  EXPECT_FALSE(s.ok());
}

}  // namespace

}  // namespace pasta

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
