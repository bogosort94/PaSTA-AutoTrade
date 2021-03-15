cc_library(
  name = "data_client",
  hdrs = ["data_client.h"],
  srcs = ["data_client.cc"],
  deps = [
      "@absl//absl/container:flat_hash_map",
      "@absl//absl/flags:flag",
      "@absl//absl/status",
      "@glog//:glog",
  ],
  linkopts = ["-lpthread",
              "-lboost_system",
              "-lssl",
              "-lcrypto",],
)

cc_test(
  name = "data_client_test",
  srcs = ["data_client_test.cc"],
  deps = [
      ":data_client",
      "@absl//absl/flags:flag",
      "@glog//:glog",
      "@gtest//:gtest",
  ],
)
