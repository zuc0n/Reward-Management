#pragma once

#include <string>
#include <optional>
#include <vector>
#include "models/Transaction.h"

namespace storage {

class TransactionStorage {
public:
    // Save transaction to data/transactions/{transaction_id}.json
    static bool save(const models::Transaction& tx);
    // Load transaction from data/transactions/{transaction_id}.json
    static std::optional<models::Transaction> load(const std::string& transaction_id);
    // List all transactions from data/transactions/*.json
    static std::vector<models::Transaction> listAll();
};

} // namespace storage 