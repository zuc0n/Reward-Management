#include "storage/WalletStorage.h"
#include "storage/FileManager.h"
#include <nlohmann/json.hpp>
#include <filesystem>

namespace storage {
namespace fs = std::filesystem;

bool WalletStorage::save(const models::Wallet& wallet) {
    nlohmann::json j = wallet;
    std::string path = "data/wallets/" + wallet.wallet_id + ".json";
    return FileManager::writeJson(path, j);
}

std::optional<models::Wallet> WalletStorage::load(const std::string& wallet_id) {
    std::string path = "data/wallets/" + wallet_id + ".json";
    nlohmann::json j;
    if (!FileManager::readJson(path, j)) return std::nullopt;
    try {
        models::Wallet w = j.get<models::Wallet>();
        return w;
    } catch (...) {
        return std::nullopt;
    }
}

std::vector<models::Wallet> WalletStorage::listAll() {
    std::vector<models::Wallet> wallets;
    std::string dir = "data/wallets";
    if (!fs::exists(dir)) return wallets;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".json") {
            nlohmann::json j;
            if (FileManager::readJson(entry.path().string(), j)) {
                try {
                    wallets.push_back(j.get<models::Wallet>());
                } catch (...) {}
            }
        }
    }
    return wallets;
}

} // namespace storage 