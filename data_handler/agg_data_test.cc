#include "data_handler/agg_data.h"

#include "absl/status/status.h"
#include "data_handler/data_handler_testutil.h"
#include "glog/logging.h"
#include "google/protobuf/util/json_util.h"
#include "gtest/gtest.h"
#include "proto/data.pb.h"

namespace pasta {

namespace {

class AggregateDataTest : public ::testing::Test {
 public:
  void SetUp() override {
    one_sec.Clear();
    ten_sec.Clear();
  }
 protected:
  AggregateDataProto GetAggDataProtoFromString(
      const std::string& agg_data_str) {
    AggregateDataProto proto;
    google::protobuf::util::Status s =
        google::protobuf::util::JsonStringToMessage(agg_data_str, &proto);
    CHECK(s.ok());
    return proto;
  }

  AggDataStore one_sec = AggDataStore(1);
  AggDataStore ten_sec = AggDataStore(10);
};

TEST_F(AggregateDataTest, UpdateAgg) {
  AggregateData agg(GetAggDataProtoFromString(kTestCases_1[0]));
  agg.UpdateWith(GetAggDataProtoFromString(kTestCases_1[1]));
  agg.UpdateWith(GetAggDataProtoFromString(kTestCases_1[2]));
  AggregateData result(GetAggDataProtoFromString(kResult_1));
  EXPECT_EQ(agg, result);
}

TEST_F(AggregateDataTest, UpdateWrongTicker) {
  AggregateData agg(GetAggDataProtoFromString(kTestCases_1[0]));
  ASSERT_DEATH(agg.UpdateWith(GetAggDataProtoFromString(kTestCase_2)), "");
}

TEST_F(AggregateDataTest, OneSecAlwaysClose) {
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(one_sec.AddData(GetAggDataProtoFromString(kTestCases_1[i])));
  }
  EXPECT_TRUE(one_sec.AddData(GetAggDataProtoFromString(kTestCase_3)));
  EXPECT_EQ(one_sec.GetData("SPCE").size(), 4);
  EXPECT_TRUE(one_sec.GetData("AAPL").empty());
}

TEST_F(AggregateDataTest, MultiSecCloseAndAlignment) {
  EXPECT_FALSE(ten_sec.AddData(GetAggDataProtoFromString(kTestCases_1[0])));
  EXPECT_TRUE(ten_sec.AddData(GetAggDataProtoFromString(kTestCases_1[1])));
  EXPECT_FALSE(ten_sec.AddData(GetAggDataProtoFromString(kTestCases_1[2])));
  auto data = ten_sec.GetData("SPCE");
  EXPECT_EQ(data.size(), 2);
  EXPECT_EQ(data[0].start_, 1610144870000);
  EXPECT_EQ(data[1].start_, 1610144860000);
}

}  // namespace

}  // namespace pasta

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
