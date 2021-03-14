cc_library(
  name = "data_client",
  hdrs = ["data_client.h"],
  srcs = ["data_client.cc"],
  deps = [
      "@com_google_absl//absl/container:flat_hash_map",
      "@com_google_absl//absl/flags:flag",
      "@com_google_absl//absl/status",
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
      "@com_google_absl//absl/flags:flag",
      "@gtest//:gtest",
  ],
)
