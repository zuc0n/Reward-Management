#include "services/WalletService.h"
#include "storage/WalletStorage.h"
#include "storage/TransactionStorage.h"

#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

namespace services {

std::optional<std::string> WalletService::createWallet(const std::string& username) {
    // Generate unique wallet ID
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;
    uint64_t rnd = distr(eng);
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream oss;
    oss << std::hex << rnd << now;
    std::string walletId = oss.str();

    models::Wallet wallet(walletId, username, 0.0);
    bool ok = storage::WalletStorage::save(wallet);
    if (!ok) return std::nullopt;
    return walletId;
}

std::optional<models::Wallet> WalletService::getWallet(const std::string& walletId) {
    return storage::WalletStorage::load(walletId);
}

bool WalletService::executeTransaction(const std::string& walletId,
                                       double amount,
                                       const std::string& type,
                                       const std::string& description) {
    auto walletOpt = storage::WalletStorage::load(walletId);
    if (!walletOpt) return false;
    auto wallet = *walletOpt;

    // Apply transaction
    if (type == "debit") {
        if (wallet.balance < amount) {
            return false;
        }
        wallet.balance -= amount;
    } else if (type == "credit") {
        wallet.balance += amount;
    } else {
        return false;
    }

    // Generate transaction ID
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;
    uint64_t rnd = distr(eng);
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream oss;
    oss << std::hex << rnd << now;
    std::string txId = oss.str();

    // Timestamp as seconds since epoch
    auto ts = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::string timestamp = std::to_string(ts);

    // Create transaction record
    models::Transaction tx(txId, walletId, amount, timestamp, type, description);
    if (!storage::TransactionStorage::save(tx)) {
        return false;
    }

    // Update wallet record
    wallet.transaction_ids.push_back(txId);
    return storage::WalletStorage::save(wallet);
}

std::vector<models::Transaction> WalletService::getTransactions(const std::string& walletId) {
    std::vector<models::Transaction> result;
    auto walletOpt = storage::WalletStorage::load(walletId);
    if (!walletOpt) return result;
    for (const auto& txId : walletOpt->transaction_ids) {
        auto txOpt = storage::TransactionStorage::load(txId);
        if (txOpt) {
            result.push_back(*txOpt);
        }
    }
    return result;
}

} // namespace services 