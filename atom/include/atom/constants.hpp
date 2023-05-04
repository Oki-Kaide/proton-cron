#pragma once

using namespace eosio;
using namespace std;

#define ORACLE_CONTRACT name("oracles")
#define SYSTEM_CONTRACT name("eosio")
#define SYSTEM_TOKEN_CONTRACT name("eosio.token")
#define SYSTEM_TOKEN_SYMBOL symbol("XPR", 4)
#define CHARGE_PER_CALL asset(1 * 10000, SYSTEM_TOKEN_SYMBOL)

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
} // namespace proton