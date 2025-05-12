#include "storage/UserStorage.h"
#include "storage/FileManager.h"
#include <nlohmann/json.hpp>
#include <filesystem>

namespace storage {
namespace fs = std::filesystem;

bool UserStorage::save(const models::UserAccount& user) {
    nlohmann::json j = user;
    std::string path = "data/users/" + user.username + ".json";
    return FileManager::writeJson(path, j);
}

std::optional<models::UserAccount> UserStorage::load(const std::string& username) {
    std::string path = "data/users/" + username + ".json";
    nlohmann::json j;
    if (!FileManager::readJson(path, j)) return std::nullopt;
    try {
        models::UserAccount u = j.get<models::UserAccount>();
        return u;
    } catch (...) {
        return std::nullopt;
    }
}

std::vector<models::UserAccount> UserStorage::listAll() {
    std::vector<models::UserAccount> users;
    std::string dir = "data/users";
    if (!fs::exists(dir)) return users;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".json") {
            nlohmann::json j;
            if (FileManager::readJson(entry.path().string(), j)) {
                try {
                    users.push_back(j.get<models::UserAccount>());
                } catch (...) {}
            }
        }
    }
    return users;
}

} // namespace storage 