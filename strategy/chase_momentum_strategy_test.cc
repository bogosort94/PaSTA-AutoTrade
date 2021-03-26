#include "strategy/chase_momentum_strategy.h"

#include "data_handler/data_handler_testutil.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

#include <functional>

namespace pasta {

namespace {

class ChaseMomentumStrategyTest : public ::testing::Test {
 protected:
  DataHandler dh = DataHandler(nullptr);
  ChaseMomentumStrategy s = ChaseMomentumStrategy(&dh);
};

TEST_F(ChaseMomentumStrategyTest, AlpacaGetAccount) {
  EXPECT_EQ(s.Init(), absl::OkStatus());
}

std::string kTestCase_1[] = {
    MakeAggProto("A", "A", 200, 8642007, 25.66, 25.3981, 2.39, 2.45, 2.57, 2.35,
                 25.4014, 50, 1610114868000, 1610114869000),
    MakeAggProto("A", "A", 7000, 8642007, 25.66, 25.3981, 2.48, 3.45, 3.50,
                 2.44, 25.4014, 50, 1610114871000, 1610114872000),
};

std::string kTestCase_2[] = {
    MakeAggProto("A", "B", 200, 8642007, 25.66, 25.3981, 2.39, 12.45, 12.57,
                 2.35, 25.4014, 50, 1610114868000, 1610114869000),
};

std::string kTestCase_3[] = {
    MakeAggProto("A", "C", 200, 8642007, 25.66, 25.3981, 2.39, 12.45, 12.57,
                 2.35, 25.4014, 50, 1610134868000, 1610134869000),
};

TEST_F(ChaseMomentumStrategyTest, EntryPoint) {
  dh.ProcessMessage(GetMessage({kTestCase_1[0], kTestCase_2[0]}));
  EXPECT_FALSE(s.IsEntryPoint("A"));
  EXPECT_FALSE(s.IsEntryPoint("B"));
  dh.ProcessMessage(GetMessage({kTestCase_1[1]}));
  EXPECT_TRUE(s.IsEntryPoint("A"));
  dh.ProcessMessage(GetMessage({kTestCase_3[0]}));
  EXPECT_FALSE(s.IsEntryPoint("C"));
}

}  // namespace

}  // namespace pasta

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
