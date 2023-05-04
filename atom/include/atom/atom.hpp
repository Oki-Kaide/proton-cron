#pragma once

// Standard.
#include <vector>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

// Local
#include "constants.hpp"
#include "tables.hpp"

using namespace eosio;
using namespace std;

namespace proton {
  CONTRACT atom : public contract {
  public:
    using contract::contract;

    atom( name receiver, name code, datastream<const char*> ds )
      : contract(receiver, code, ds),
        _crons(receiver, receiver.value),
        _globals(receiver, receiver.value) {}

    ACTION addcron (
      const name& account,
      const name& contract,
      const time_point& last_process,
      const uint64_t& seconds_interval
    );
    ACTION deletecron ( const name& account, const uint64_t& cron_index );
    ACTION withdraw ( const name& account, const uint64_t& cron_index );
    ACTION process (
      const name& account,
      const uint64_t& max
    );

    // ACTION cleanup () {
    //   require_auth(get_self());
      
    //   cron_table db(get_self(), get_self().value);
    //   auto itr = db.end();
    //   while(db.begin() != itr){
    //     itr = db.erase(--itr);
    //   }
    // };

    // Just a test function
    ACTION cron () {
      require_auth(get_self());
      print(get_config().txs_sent);
    };

    // This function will be called when the contract is notified of incoming or outgoing transfer actions from any contract
    [[eosio::on_notify("eosio.token::transfer")]]
    void ontransfer   ( const name& from,
                        const name& to,
                        const asset& quantity,
                        const string& memo );

    // Action wrappers
    using addcron_action = action_wrapper<"addcron"_n, &atom::addcron>;
    using transfer_action = action_wrapper<"transfer"_n, &atom::ontransfer>;

    // Initialize tables from tables.hpp
    cron_table _crons;
    global_table _globals;

    globals get_config () {
      globals glob;
      if (_globals.exists()) {
        glob = _globals.get();
      } else {
        glob = globals{};
        _globals.set(glob, get_self());
      }
      return glob;
    }

    uint64_t next_sender_id() {
      auto global = get_config();
      auto temp_id = global.txs_sent;
      global.txs_sent++;
      _globals.set(global, get_self());
      return temp_id;
    }
  private:
    void fundcron(const uint64_t& cron_index, const asset& fund);
    void process_deferred(const name& contract, const name& ram_payer, const uint64_t& delay);
  };
}