# Proton Longstaking Server

Currently only provides XPR/BTC oracle (change constants and pm2 configs to add new oracles)

1. Install npm and nodejs:

```sh
curl -Ls install-node.now.sh | sh
```

2. Install PM2

```sh
npm i -g pm2
```

3. Install (in server folder)

```sh
npm i && npm run build
```

4. Rename .example.env to .env and set private key and accounts

5. Run oracle

```sh
# Testnet
pm2 start testnet.config.js

# Mainnet
pm2 start testnet.config.js
```