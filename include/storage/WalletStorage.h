#pragma once

#include <string>
#include <optional>
#include <vector>
#include "models/Wallet.h"

namespace storage {

class WalletStorage {
public:
    // Save wallet to data/wallets/{wallet_id}.json
    static bool save(const models::Wallet& wallet);
    // Load wallet from data/wallets/{wallet_id}.json
    static std::optional<models::Wallet> load(const std::string& wallet_id);
    // List all wallets from data/wallets/*.json
    static std::vector<models::Wallet> listAll();
};

} // namespace storage 