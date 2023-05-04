#pragma once

using namespace eosio;
using namespace std;

namespace proton {
  struct [[eosio::table, eosio::contract("atom")]] globals {
    uint64_t txs_sent = 0;
    uint64_t reserved1 = 0;
    uint64_t reserved2 = 0;
    uint64_t reserved3 = 0;
    uint64_t reserved4 = 0;
    uint64_t reserved5 = 0;
  };
  typedef eosio::singleton<"globals"_n, globals> global_table;

  struct [[eosio::table, eosio::contract("atom")]] Cron {
    uint64_t index;
    name account;
    name contract;
    asset balance;
    time_point last_process;
    uint64_t seconds_interval;

    uint64_t primary_key() const { return index; };
    uint64_t time_left() const {
      auto insufficient_balance = balance <= CHARGE_PER_CALL;
      if (insufficient_balance) {
        return numeric_limits<uint64_t>::max();
      }

      uint64_t start = last_process.sec_since_epoch() + seconds_interval;
      uint64_t current = current_time_point().sec_since_epoch();

      return start >= current
        ? start - current
        : 0;
    };
  };

  typedef multi_index<"crons"_n, Cron,
    indexed_by<"bytime"_n, const_mem_fun<Cron, uint64_t, &Cron::time_left>>
  > cron_table;
}