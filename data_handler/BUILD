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

cc_library(
    name = "agg_data",
    hdrs = ["agg_data.h"],
    srcs = ["agg_data.cc"],
    deps = [
      "//proto:data_cc_proto",
      "@absl//absl/container:flat_hash_map",
      "@absl//absl/flags:flag",
      "@absl//absl/status",
      "@glog//:glog",
    ],
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
  linkopts = ["-lboost_system",],
)

cc_test(
    name = "agg_data_test",
    srcs = ["agg_data_test.cc"],
    deps = [
      ":agg_data",
      "@absl//absl/status",
      "@com_google_protobuf//:protobuf",
      "@glog//:glog",
      "@gtest//:gtest",
    ],
)
