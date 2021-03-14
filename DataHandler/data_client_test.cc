#include "DataHandler/data_client.h"

#include "absl/flags/flag.h"
#include "gtest/gtest.h"

namespace pasta {

namespace {

class DataClientTest : public ::testing::Test {};

TEST_F(DataClientTest, ConnectAndSubscribe) {
  absl::SetFlag(&FLAGS_data_client_no_run, true);
  DataClient dc;
  dc.SetAuthentication("U0xZEcN2ecwK6PqW8jVMH6F5MniB1GKN");
  absl::Status s = dc.Run();
  EXPECT_TRUE(s.ok());
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
