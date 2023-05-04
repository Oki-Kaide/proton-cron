#include <atom/atom.hpp>

namespace proton
{
  void atom::addcron (
    const name& account,
    const name& contract,
    const time_point& last_process,
    const uint64_t& seconds_interval
  ) {
    require_auth(account);
    check(is_account(contract), "contract is not an account");
    _crons.emplace(account, [&](auto& c) {
      c.index = _crons.available_primary_key();
      c.account = account;
      c.contract = contract;
      c.balance = asset(0, SYSTEM_TOKEN_SYMBOL);
      c.last_process = last_process;
      c.seconds_interval = seconds_interval;
    });
  }

  void atom::deletecron (
    const name& account,
    const uint64_t& cron_index
  ) {
    require_auth(account);

    auto cron = _crons.require_find(cron_index, "cron not found");
    check(cron->account == account, "accounts dont match for cron");
    _crons.erase(cron);
  };

  void atom::withdraw (
    const name& account,
    const uint64_t& cron_index
  ) {
    require_auth(account);

    // Find Cron
    auto cron = _crons.require_find(cron_index, "cron not found");
    check(cron->account == account, "accounts dont match for cron");
    check(cron->balance.amount > 0, "balance too low to withdraw");

    // Send token 
    transfer_action t_action(SYSTEM_TOKEN_CONTRACT, {get_self(), name("active")} );
    t_action.send(get_self(), account, cron->balance, "Cron Withdraw");

    // Substract balance
    _crons.modify(cron, same_payer, [&](auto& c) {
      c.balance.amount = 0;
    });
  };

  void atom::process (
    const name& account,
    const uint64_t& max
  ) {
    require_auth(account);

    auto idx = _crons.get_index<"bytime"_n>();
    auto itr = idx.lower_bound(0);
    auto oitr = itr;

    check(itr != idx.end() && itr->time_left() == 0, "no crons to process");

    for (uint16_t i = 0; i < max; ++i) {
      itr = oitr;
      if (itr == idx.end() || itr->time_left() > 0) break;

      // Charge
      check(itr->balance >= CHARGE_PER_CALL, "balance too low.");
      idx.modify(itr, same_payer, [&](auto& c) {
        c.balance -= CHARGE_PER_CALL;
        c.last_process = current_time_point();
      });

      // Payout to processor
      transfer_action t_action(SYSTEM_TOKEN_CONTRACT, {get_self(), name("active")} );
      t_action.send(get_self(), account, CHARGE_PER_CALL, "Cron Processor Payment!");

      // Deferred Call
      process_deferred(itr->contract, account, 0);
    }
  }

  void atom::process_deferred(const name& contract, const name& ram_payer, const uint64_t& delay)
  {
    transaction t{};
    t.actions.emplace_back(
      permission_level(get_self(), name("active")),
      contract,
      name("cron"),
      std::make_tuple()
    );

    // set delay in seconds
    t.delay_sec = delay;

    // first argument is a unique sender id
    // second argument is account paying for RAM
    // third argument can specify whether an in-flight transaction with this senderId should be replaced
    // if set to false and this senderId already exists, this action will fail
    t.send(next_sender_id(), ram_payer, false);
  }
} // namepsace contract