#include "data_handler/data_handler.h"

#include "absl/status/status.h"
#include "data_handler/data_handler_testutil.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

#include <vector>

namespace pasta {

namespace {

class DataHandlerTest : public ::testing::Test {
 protected:
  std::string GetMessage(std::vector<std::string> test_cases) {
    CHECK(!test_cases.empty());
    std::string msg = "[" + test_cases[0];
    for (int i = 1; i < test_cases.size(); ++i) {
      msg += "," + test_cases[i];
    }
    msg += "]";
    return msg;
  }

  DataHandler dh = DataHandler(nullptr);
};

TEST_F(DataHandlerTest, CallbackCount) {
  int cb_count = 0;
  EXPECT_EQ(
      dh.RegisterCallback("count_callback",
                          [&cb_count](std::string ticker) { ++cb_count; }),
      absl::OkStatus());

  dh.ProcessMessage(GetMessage({kTestCases_1[0]}));
  dh.ProcessMessage(GetMessage({kTestCases_1[1], kTestCase_2}));
  dh.ProcessMessage(GetMessage({kTestCases_1[2]}));

  EXPECT_EQ(cb_count, 4);
}

}  // namespace
}  // namespace pasta

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
