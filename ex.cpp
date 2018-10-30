#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::receivedenu(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get USD balance
  double usd_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(USD_SYMBOL).name()).amount;
  usd_balance = usd_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUUSD_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double product_old = usd_balance * enu_balance;
  double product_new = usd_balance * (enu_balance+received);

  double new_shares = (shares*(product_new - product_old))/product_old;

  auto quantity = asset(10000*new_shares, ENUUSD_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUUSD shares")))
      .send();

  action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
         std::make_tuple(_self, N(enu.usd.mm), transfer.quantity,
                         std::string("Invest ENUUSD shares with ENU")))
      .send();
}

void ex::receivedusd(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get USD balance
  double usd_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(USD_SYMBOL).name()).amount;
  usd_balance = usd_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUUSD_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double product_old = usd_balance * enu_balance;
  double product_new = (usd_balance+received) * enu_balance;

  double new_shares = (shares*(product_new - product_old))/product_old;

  auto quantity = asset(10000*new_shares, ENUUSD_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUUSD shares")))
      .send();

  action(permission_level{_self, N(active)}, N(stable.coin), N(transfer),
         std::make_tuple(_self, N(enu.usd.mm), transfer.quantity,
                         std::string("Invest ENUUSD shares with ENU")))
      .send();
}

void ex::receivedshares(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get USD balance
  double usd_balance = enumivo::token(N(stable.coin)).
	   get_balance(N(enu.usd.mm), enumivo::symbol_type(USD_SYMBOL).name()).amount;
  usd_balance = usd_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUUSD_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  auto usdportion = asset(10000*usd_balance*(received/shares), USD_SYMBOL);

  action(permission_level{N(enu.usd.mm), N(active)}, N(stable.coin), N(transfer),
         std::make_tuple(N(enu.usd.mm), transfer.from, usdportion,
                         std::string("Divest ENUUSD shares for USD")))
      .send();

  auto enuportion = asset(10000*enu_balance*(received/shares), ENU_SYMBOL);

  action(permission_level{N(enu.usd.mm), N(active)}, N(enu.token), N(transfer),
         std::make_tuple(N(enu.usd.mm), transfer.from, enuportion,
                         std::string("Divest ENUUSD shares for ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(shares.coin), N(retire),
         std::make_tuple(transfer.quantity, std::string("Retire ENUUSD shares")))
      .send();
}

void ex::apply(account_name contract, action_name act) {

  if (contract == N(enu.token) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENU_SYMBOL,
                 "Must send ENU");
    receivedenu(transfer);
    return;
  }

  if (contract == N(stable.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == USD_SYMBOL,
                 "Must send USD");
    receivedusd(transfer);
    return;
  }

  if (contract == N(shares.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENUUSD_SYMBOL,
                 "Must send ENUUSD");
    receivedshares(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "Must send USD or ENU or ENUUSD");
    return;
  }

  if (contract != _self) return;

}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex enuusdshares(receiver);
  enuusdshares.apply(code, action);
  enumivo_exit(0);
}
}
