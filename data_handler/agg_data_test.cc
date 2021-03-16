#include "data_handler/agg_data.h"

#include "absl/status/status.h"
#include "glog/logging.h"
#include "google/protobuf/util/json_util.h"
#include "gtest/gtest.h"
#include "proto/data.pb.h"

namespace pasta {

namespace {

class AggregateDataTest : public ::testing::Test {
 protected:
  AggregateDataProto GetAggDataProtoFromString(
      const std::string& agg_data_str) {
    AggregateDataProto proto;
    google::protobuf::util::Status s =
        google::protobuf::util::JsonStringToMessage(agg_data_str, &proto);
    CHECK(s.ok());
    return proto;
  }
};

std::string kTestCases_1[] = {
    R"tag({
   "ev": "A",
   "sym": "SPCE",
   "v": 200,
   "av": 8642007,
   "op": 25.66,
   "vw": 25.3981,
   "o": 25.39,
   "c": 25.45,
   "h": 25.57,
   "l": 25.35,
   "a": 25.4014,
   "z": 50,
   "s": 1610144868000,
   "e": 1610144869000
  })tag",
    R"tag({
   "ev": "A",
   "sym": "SPCE",
   "v": 100,
   "av": 8642107,
   "op": 25.66,
   "vw": 25.3980,
   "o": 25.43,
   "c": 25.47,
   "h": 25.49,
   "l": 25.30,
   "a": 25.4714,
   "z": 50,
   "s": 1610144869000,
   "e": 1610144870000
  })tag",
    R"tag({
   "ev": "A",
   "sym": "SPCE",
   "v": 600,
   "av": 8642707,
   "op": 25.66,
   "vw": 25.3983,
   "o": 25.45,
   "c": 25.54,
   "h": 25.61,
   "l": 25.44,
   "a": 25.5714,
   "z": 50,
   "s": 1610144870000,
   "e": 1610144871000
  })tag",
};

std::string kResult_1 =
    R"tag({
   "ev": "A",
   "sym": "SPCE",
   "v": 900,
   "av": 8642707,
   "op": 25.66,
   "vw": 25.3983,
   "o": 25.39,
   "c": 25.54,
   "h": 25.61,
   "l": 25.30,
   "a": 25.5714,
   "z": 50,
   "s": 1610144868000,
   "e": 1610144871000
  })tag";

std::string kTestCase_2 =
    R"tag({
   "ev": "A",
   "sym": "AAPL",
   "v": 100,
   "av": 8642107,
   "op": 25.66,
   "vw": 25.3980,
   "o": 25.43,
   "c": 25.47,
   "h": 25.49,
   "l": 25.30,
   "a": 25.4714,
   "z": 50,
   "s": 1610144869000,
   "e": 1610144870000
  })tag";

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

}  // namespace

}  // namespace pasta

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
