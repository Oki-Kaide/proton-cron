# Proton CRON

## Usage

1. Add "cron" action to your contract:
```c++
ACTION cron () {
  require_auth("cron");
  // do whatever
}
```

1. Create daily CRON Job (replace params with your own)

`account`: account creating cron

`contract`: account your contract is deployed to

`last_process`: date to start processing from

`seconds_interval`: time between each process (roughly, never exact)

```sh
cleos -u https://proton.greymass.com push transaction '{
  "actions": [
    {
      "account": "cron",
      "name": "addcron",
      "data": {
        "account": "syed",
        "contract": "cron",
        "last_process": "1970-01-01T00:26:50.347",
        "seconds_interval": 86400
      },
      "authorization": [
        {
          "actor": "syed",
          "permission": "active"
        }
      ]
    }
  ]
}'
```

1. View tables to get cron `index`
```
cleos -u https://proton.greymass.com get table cron cron crons
```

4. Fund the CRON Job (1 XPR per call, set cron index as memo)
```sh
cleos -u https://proton.greymass.com push action eosio.token transfer '["syed", "cron", "2.0000 XPR", "0"]' -p syed
```

5. Anyone can call `process`, and earn 1 XPR per processed cron
```sh
cleos -u https://proton.greymass.com push action cron process '["syed", "5"]' -p syed
```