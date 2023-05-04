export const CHAIN = process.env.CHAIN
export const PRIVATE_KEYS = [process.env.PRIVATE_KEY]
if (!CHAIN) {
    console.error('No CHAIN provided')
    process.exit(0)
}
if (!PRIVATE_KEYS.length) {
    console.error('No PRIVATE_KEY provided in .env')
    process.exit(0)
}

export const ENDPOINTS = CHAIN === 'proton'
    ? ["https://proton.eoscafeblock.com", "https://proton.eosusa.news", "https://proton.cryptolions.io", "https://proton.pink.gg", "https://proton.eoscafeblock.com"]
    : ["https://testnet.protonchain.com", "https://testnet.proton.pink.gg"]


export const CRON_CONTRACT = "cron"
export const WAIT_TIME = 15 * 1000
export const ACCOUNT = 'cronhelper'
export const ACCOUNT_PERMISSION = 'active'