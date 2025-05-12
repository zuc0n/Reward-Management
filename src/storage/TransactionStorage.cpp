#include "storage/TransactionStorage.h"
#include "storage/FileManager.h"
#include <nlohmann/json.hpp>
#include <filesystem>

namespace storage {
namespace fs = std::filesystem;

bool TransactionStorage::save(const models::Transaction& tx) {
    nlohmann::json j = tx;
    std::string path = "data/transactions/" + tx.transaction_id + ".json";
    return FileManager::writeJson(path, j);
}

std::optional<models::Transaction> TransactionStorage::load(const std::string& transaction_id) {
    std::string path = "data/transactions/" + transaction_id + ".json";
    nlohmann::json j;
    if (!FileManager::readJson(path, j)) return std::nullopt;
    try {
        models::Transaction t = j.get<models::Transaction>();
        return t;
    } catch (...) {
        return std::nullopt;
    }
}

std::vector<models::Transaction> TransactionStorage::listAll() {
    std::vector<models::Transaction> transactions;
    std::string dir = "data/transactions";
    if (!fs::exists(dir)) return transactions;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".json") {
            nlohmann::json j;
            if (FileManager::readJson(entry.path().string(), j)) {
                try {
                    transactions.push_back(j.get<models::Transaction>());
                } catch (...) {}
            }
        }
    }
    return transactions;
}

} // namespace storage 