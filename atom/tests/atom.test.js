const { loadConfig, Blockchain } = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");

const CRON = 'cron'
const CRONS_TABLE = 'crons'

describe(CRON, () => {
  let blockchain = new Blockchain(config);
  let cron = blockchain.createAccount(CRON);
  let token = blockchain.createAccount(`eosio.token`);
  let user = blockchain.createAccount(`user`);

  const userAuth = [{ actor: user.accountName, permission: 'active' }]

  beforeAll(async () => {
    cron.setContract(blockchain.contractTemplates[CRON]);
    cron.updateAuth(`active`, `owner`, { accounts: [{ permission: { actor: cron.accountName, permission: `eosio.code` }, weight: 1 }] });

    token.setContract(blockchain.contractTemplates[`eosio.token`]);
  });

  beforeEach(async () => {
    cron.resetTables();

    token.resetTables();
    await token.loadFixtures();

    blockchain.setCurrentTime(new Date())
  });

  const last_date = new Date()
  const last_process = last_date.toISOString().slice(0, -1)

  it("Create Cron (addcron)", async () => {
    expect.assertions(1);

    const cronObj = {
      account: user.accountName,
      contract: cron.accountName,
      last_process,
      seconds_interval: "10"
    }
    await cron.contract.addcron(cronObj, userAuth);

    expect(cron.getTableRowsScoped(CRONS_TABLE)[CRON]).toEqual(
      [{
        index: "0",
        balance: "0.0000 XPR",
        ...cronObj
      }]
    );
  });

  it("Delete Cron (deletecron)", async () => {
    expect.assertions(1);

    const cronObj = {
      account: user.accountName,
      contract: cron.accountName,
      last_process,
      seconds_interval: "10"
    }
    await cron.contract.addcron(cronObj, userAuth);
    await cron.contract.deletecron({
      account: user.accountName,
      cron_index: "0"
    }, userAuth);

    expect(cron.getTableRowsScoped(CRONS_TABLE)[CRON]).toEqual(undefined);
  });

  it("Deposit Funds", async () => {
    expect.assertions(1);

    const cronObj = {
      account: user.accountName,
      contract: cron.accountName,
      last_process,
      seconds_interval: "10"
    }
    await cron.contract.addcron(cronObj, userAuth);
    await token.contract.transfer({
      from: user.accountName,
      to: cron.accountName,
      quantity: '1.0000 XPR',
      memo: '0'
    }, userAuth)

    expect(cron.getTableRowsScoped(CRONS_TABLE)[CRON]).toEqual(
      [{
        index: "0",
        balance: "1.0000 XPR",
        ...cronObj
      }]
    );
  });

  it("Withdraw Funds", async () => {
    expect.assertions(3);

    const cronObj = {
      account: user.accountName,
      contract: cron.accountName,
      last_process,
      seconds_interval: "10"
    }
    await cron.contract.addcron(cronObj, userAuth);
    await token.contract.transfer({
      from: user.accountName,
      to: cron.accountName,
      quantity: '1.0000 XPR',
      memo: '0'
    }, userAuth)

    expect(token.getTableRowsScoped('accounts')[user.accountName]).toEqual(
      [{
        balance: '999999.0000 XPR'
      }]
    );

    await cron.contract.withdraw({
      account: user.accountName,
      cron_index: "0"
    }, userAuth);

    expect(cron.getTableRowsScoped(CRONS_TABLE)[CRON]).toEqual(
      [{
        index: "0",
        balance: "0.0000 XPR",
        ...cronObj
      }]
    );

    expect(token.getTableRowsScoped('accounts')[user.accountName]).toEqual(
      [{
        balance: '1000000.0000 XPR'
      }]
    );
  });

  it("Test Cron", async () => {
    expect.assertions(2);

    await cron.contract.addcron({
      account: user.accountName,
      contract: cron.accountName,
      last_process,
      seconds_interval: "86400"
    }, userAuth);

    // 1 Day forward
    blockchain.setCurrentTime(new Date(last_date.getTime() + 60 * 60 * 24 * 1000))

    // Fund it
    await token.contract.transfer({
      from: user.accountName,
      to: cron.accountName,
      quantity: '1.0000 XPR',
      memo: '0'
    }, userAuth)

    // Process
    await cron.contract.process({
      account: user.accountName,
      max: "1"
    }, userAuth);

    // Deferred
    const deferredTransactions = blockchain.getDeferredTransactions();
    expect(deferredTransactions).toMatchObject([
      {
        actions: [
          {
            account: cron.accountName,
            name: "cron",
            authorization: [{
              actor: cron.accountName,
              permission: "active"
            }],
            data: {}
          }
        ],
        delay_sec: 0
      }
    ]);
  
    // Execute deferred Transaction
    const txTrace = await blockchain.sendTransaction(deferredTransactions[0]);
    const consoleOutput = txTrace.action_traces[0].console;
    expect(consoleOutput).toBe("1");
  });
});
