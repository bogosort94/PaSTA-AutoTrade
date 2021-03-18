#include "data_handler/data_handler_testutil.h"

#include <boost/format.hpp>

namespace pasta {

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

}  // namespace pasta
