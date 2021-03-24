#include "strategy/chase_momentum_strategy.h"

#include "absl/status/status.h"
#include "absl/time/time.h"
#include "alpaca/alpaca.h"
#include "data_handler/agg_data.h"
#include "data_handler/data_handler.h"
#include "glog/logging.h"
#include "strategy/strategy.h"

namespace pasta {

ChaseMomentumStrategy::ChaseMomentumStrategy(DataHandler* dh)
    : Strategy(dh), trading_(""), dry_run_(false) {
  LOG(INFO) << dh << " vs " << dh_;
  absl::LoadTimeZone("America/New_York", &nyc_);
}

absl::Status ChaseMomentumStrategy::Init() {
  auto env = alpaca::Environment();
  if (auto status = env.parse(); !status.ok()) {
    return absl::AbortedError(absl::StrCat(
        "Alpaca environment parsing failure (code ",
        std::to_string(status.getCode()), "): ", status.getMessage()));
  }
  client_ = std::make_unique<alpaca::Client>(env);

  auto account_response = client_->getAccount();
  if (auto status = account_response.first; !status.ok()) {
    return absl::AbortedError(absl::StrCat(
        "Alpaca getting account information", "failure (code ",
        std::to_string(status.getCode()), "): ", status.getMessage()));
  }
  account_ = std::make_unique<alpaca::Account>(account_response.second);

  if (account_->trading_blocked) {
    return absl::AbortedError("Account is currently resitricted from trading.");
  }

  if (account_->shorting_enabled) {
    return absl::AbortedError(
        "Shorting is enabled for this account. Please "
        "disable shorting for safety.");
  }

  LOG(INFO) << account_->cash << " is available as cash.";
  LOG(INFO) << account_->buying_power << " is available as buying power.";

  dh_->RegisterCallback("ChaseMomentumStrategy process new data",
                        std::bind(&ChaseMomentumStrategy::ProcessNewData, this,
                                  std::placeholders::_1));

  return absl::OkStatus();
}

void ChaseMomentumStrategy::ProcessNewData(const std::string& ticker) {
  if (trading_.empty()) {
    MaybeEnterTrade(ticker);
  } else if (ticker == trading_) {
    PositionManagement();
  } else {
    // Do nothing because currently we only allow trading one stock at a time.
  }
}

void ChaseMomentumStrategy::MaybeEnterTrade(const std::string& ticker) {
  if (IsEntryPoint(ticker)) {
    quantity_ = 0;
    trading_ = ticker;
    enter_ts_ = dh_->GetData(TEN_SEC, ticker).front().end_;
    clear_ = false;
    EnterTrade();
    if (quantity_ <= 0) trading_.clear();
  }
}

// Enter trade when the following holds:
// 1. Time is between 9:00 am and 9:25 am. We do not allow this to happen in
//    regular hours to avoid handling halts.
// 2. Price goes up by 20% in the past two 10-second candles.
// 3. Price is between $2 and $50.
// 4. The volume of the most recent 10-second candle is greater than 3000.
// 5. The volume of the most recent 10-second candle is less than 1.5 times
//    the volume of the most recent 5-minute candle -- this indicates a sudden
//    increase of volume.
bool ChaseMomentumStrategy::IsEntryPoint(const std::string& ticker) {
  auto ten_sec = dh_->GetData(TEN_SEC, ticker);
  auto one_min = dh_->GetData(ONE_MIN, ticker);
  auto five_min = dh_->GetData(FIVE_MIN, ticker);
  auto data = ten_sec.front();
  int64_t ts = data.end_;
  absl::CivilMinute civil_time =
      absl::ToCivilMinute(absl::FromUnixMillis(ts), nyc_);

  if (civil_time.hour() != 9 || civil_time.minute() >= 25) {
    return false;
  }

  double open;
  if (ten_sec.size() > 1 && ten_sec[1].end_ == data.start_) {
    open = ten_sec[1].start_;
  } else {
    open = data.open_;
  }

  int64_t five_min_from_ts =
      one_min.front().start_ - 300 * NUM_MILLIS_PER_SECOND;
  int64_t tot_vol = 0;
  for (int i = 0; i < one_min.size() && one_min[i].start_ >= five_min_from_ts;
       ++i) {
    tot_vol += one_min[i].vol_;
  }

  return (data.close_ > 1.2 * open) && (data.close_ > 2.) &&
         (data.close_ < 50.) && (one_min.front().vol_ > 3000) &&
         (data.vol_ < 1.5 * tot_vol);
}

void ChaseMomentumStrategy::EnterTrade() {
  double limit_price = dh_->GetData(TEN_SEC, trading_).front().close_ + 0.05;
  // Always leave $25,000 cash in the account to comply with the PDT rule.
  // TODO: This resitriction can be lifted when the project is proven effective.
  // TODO: Limit number of shares / amount of capital used.
  int qty = (std::stod(account_->cash) - 25000.) * 0.9 / limit_price;
  if (qty <= 0) {
    LOG(INFO) << "Trying to buy " << trading_ << " at " << limit_price
              << ", but the account does not have enough cash for 1 share.";
    return;
  }

  LOG(INFO) << "Buying " << qty << " shares of " << trading_
            << " at limit price " << limit_price << "to chase momentum.";

  auto buy_response = client_->submitOrder(
      trading_, qty, alpaca::OrderSide::Buy, alpaca::OrderType::Limit,
      alpaca::OrderTimeInForce::Day, std::to_string(limit_price));
  if (auto status = buy_response.first; !status.ok()) {
    LOG(ERROR) << "Error submitting buy order: " << status.getMessage();
    return;
  }

  auto order = buy_response.second;
  int64_t filled = std::stoi(order.filled_qty);
  LOG(INFO) << filled << " (" << order.filled_qty << ") shares of " << trading_
            << " are filled immediately at" << order.filled_avg_price << ".";
  if (filled < qty) {
    // TODO: Error handling is too vulnerable.
    auto cancel_response = client_->cancelOrder(order.id);
    order = cancel_response.second;
    filled = std::stoi(order.filled_qty);
    LOG(INFO) << filled << " (" << order.filled_qty << ") shares of "
              << trading_ << " are filled eventually at"
              << order.filled_avg_price << ".";
  }

  quantity_ = filled;
  breakeven_ = std::stod(order.filled_avg_price);
}

void ChaseMomentumStrategy::PositionManagement() {
  if (clear_) ClearPosition();

  auto one_min = dh_->GetData(ONE_MIN, trading_);
  auto one_sec = dh_->GetData(ONE_SEC, trading_);
  if (one_min.front().start_ <= enter_ts_) {
    if (one_sec.front().low_ < one_min.front().open_) {
      LOG(INFO) << "Clearing " << trading_ << " position because price has "
                << "broken below open price of the candle: "
                << one_min.front().open_ << ".";
      clear_ = true;
      ClearPosition();
      return;
    }
  } else if (one_min[1].low_ <= enter_ts_) {
    if (one_sec.front().low_ < breakeven_) {
      LOG(INFO) << "Clearing " << trading_ << " position because price has "
                << "broken below breakeven: " << breakeven_ << ".";
      ClearPosition();
      return;
    }
  } else {
    if (one_sec.front().low_ < one_min[1].low_) {
      LOG(INFO) << "Clearing " << trading_ << " position because price has "
                << "broken below previous candle's low.";
      ClearPosition();
      return;
    }
  }

  if (one_min.front().end_ - one_min.front().start_ ==
          60 * NUM_MILLIS_PER_SECOND &&
      (one_min.front().close_ < one_min.front().open_)) {
    LOG(INFO) << "Clearing " << trading_
              << " position because the current candle closes red.";
    ClearPosition();
  }
}

void ChaseMomentumStrategy::ClearPosition() {
  double limit_price = dh_->GetData(ONE_SEC, trading_).front().low_ - 0.05;
  auto sell_response = client_->submitOrder(
      trading_, quantity_, alpaca::OrderSide::Sell, alpaca::OrderType::Limit,
      alpaca::OrderTimeInForce::Day, std::to_string(limit_price));
  if (auto status = sell_response.first; !status.ok()) {
    LOG(ERROR) << "Error submitting sell order: " << status.getMessage();
  }

  auto order = sell_response.second;
  int64_t filled = std::stoi(order.filled_qty);
  LOG(INFO) << filled << " (" << order.filled_qty << ") shares of " << trading_
            << " are filled immediately at" << order.filled_avg_price << ".";
  if (filled < quantity_) {
    // TODO: Error handling is too vulnerable.
    auto cancel_response = client_->cancelOrder(order.id);
    order = cancel_response.second;
    filled = std::stoi(order.filled_qty);
    LOG(INFO) << filled << " (" << order.filled_qty << ") shares of "
              << trading_ << " are filled eventually at"
              << order.filled_avg_price << ".";
  }

  quantity_ -= filled;
  if (quantity_ <= 0) {
    clear_ = false;
    trading_.clear();
  }
}

void ChaseMomentumStrategy::Test_SetDryRun() { dry_run_ = true; }

}  // namespace pasta
