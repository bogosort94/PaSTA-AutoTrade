cc_library(
  name = "strategy",
  hdrs = ["strategy.h"],
  srcs = ["strategy.h"],
  visibility = ["//visibility:public"],
  deps = ["//data_handler:data_handler"],
)

cc_library(
  name = "chase_momentum_strategy",
  hdrs = ["chase_momentum_strategy.h"],
  srcs = ["chase_momentum_strategy.cc"],
  visibility = ["//visibility:public"],
  deps = [
      ":strategy",
      "@//alpaca:alpaca",
  ],
  linkopts = ["-ldl"],
)

cc_test(
  name = "chase_momentum_strategy_test",
  srcs = ["chase_momentum_strategy_test.cc"],
  deps = [
    ":chase_momentum_strategy",
    ":strategy",
    "//data_handler:data_handler_testutil",
    "@absl//absl/status",
    "@com_github_google_glog//:glog",
    "@gtest//:gtest",
  ],
)

