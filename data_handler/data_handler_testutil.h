#ifndef PASTA_DATA_HANDLER_DATA_HANDLER_TESTUT
#define PASTA_DATA_HANDLER_DATA_HANDLER_TESTUT

#include <string>
#include <vector>

namespace pasta {

std::string MakeAggProto(std::string ev, std::string sym, int64_t v, int64_t av,
                         double op, double vw, double o, double c, double h,
                         double l, double a, int64_t z, int64_t s, int64_t e);

std::string GetMessage(std::vector<std::string> test_cases);

extern std::string kTestCases_1[];
extern std::string kResult_1;
extern std::string kTestCase_2;
extern std::string kTestCase_3;

}  // namespace pasta

#endif  // PASTA_DATA_HANDLER_DATA_HANDLER_TESTUT
