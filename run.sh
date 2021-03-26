bazel build //main:pasta_main

echo "PaSTA build complete."
echo "PaSTA Auto Trader running ..."

POLYGON_KEY=U0xZEcN2ecwK6PqW8jVMH6F5MniB1GKN \
APCA_API_KEY_ID=PKDV0RYI8PP7U4PPY6PT \
APCA_API_SECRET_KEY=ximFcibu6KV8akmYkPjhBjgsrmL7yFj7x84XdqMf \
APCA_API_BASE_URL=paper-api.alpaca.markets \
APCA_API_DATA_URL=data.alpaca.markets \
bazel-bin/main/pasta_main &> pasta.out
#bazel run //main:pasta_main
