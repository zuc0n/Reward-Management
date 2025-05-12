#pragma once

#include <string>
#include <optional>
#include <vector>
#include "models/Wallet.h"
#include "models/Transaction.h"

namespace services {

class WalletService {
public:
    // Creates a new wallet for the user, returns walletId on success
    static std::optional<std::string> createWallet(const std::string& username);

    // Retrieves a wallet by ID
    static std::optional<models::Wallet> getWallet(const std::string& walletId);

    // Executes a transaction (credit/debit) for the wallet; returns true on success
    static bool executeTransaction(const std::string& walletId,
                                   double amount,
                                   const std::string& type,
                                   const std::string& description);

    // Retrieves all transactions for a wallet
    static std::vector<models::Transaction> getTransactions(const std::string& walletId);
};

} // namespace services 