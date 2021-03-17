#include "data_handler/agg_data.h"

#include "absl/status/status.h"
#include "glog/logging.h"
#include "google/protobuf/util/json_util.h"
#include "gtest/gtest.h"
#include "proto/data.pb.h"

#include <boost/format.hpp>

namespace pasta {

namespace {

std::string MakeAggProto(std::string ev, std::string sym, int64_t v, int64_t av,
                         double op, double vw, double o, double c, double h,
                         double l, double a, int64_t z, int64_t s, int64_t e) {
  auto fmt = boost::format(
                 R"tag({"ev":"%1%","sym":"%2%","v":%3%,"av":%4%,"op":%5%,)tag"
                 R"tag("vw":%6%,"o":%7%,"c":%8%,"h":%9%,"l":%10%,"a":%11%,)tag"
                 R"tag("z":%12%,"s":%13%,"e":%14%})tag") %
             ev % sym % v % av % op % vw % o % c % h % l % a % z % s % e;
  return fmt.str();
}

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

std::string kTestCases_1[] = {
    MakeAggProto("A", "SPCE", 200, 8642007, 25.66, 25.3981, 25.39, 25.45, 25.57,
                 25.35, 25.4014, 50, 1610144868000, 1610144869000),
    MakeAggProto("A", "SPCE", 100, 8642107, 25.66, 25.3980, 25.43, 25.47, 25.49,
                 25.30, 25.4714, 50, 1610144869000, 1610144870000),
    MakeAggProto("A", "SPCE", 600, 8642707, 25.66, 25.3983, 25.45, 25.54, 25.61,
                 25.44, 25.5714, 50, 1610144870000, 1610144871000),
};

std::string kResult_1 =
    MakeAggProto("A", "SPCE", 900, 8642707, 25.66, 25.3983, 25.39, 25.54, 25.61,
                 25.30, 25.5714, 50, 1610144868000, 1610144871000);

std::string kTestCase_2 = 
    MakeAggProto("A", "AAPL", 100, 8642107, 25.66, 25.3980, 25.43, 25.47, 25.49,
                 25.30, 25.4714, 50, 1610144869000, 1610144870000);

std::string kTestCase_3 = 
    MakeAggProto("A", "SPCE", 900, 8642707, 25.66, 25.3983, 25.39, 25.54, 25.61,
                 25.30, 25.5714, 50, 1610144880000, 1610144881000);

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
