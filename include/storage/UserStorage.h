#pragma once

#include <string>
#include <optional>
#include <vector>
#include "models/UserAccount.h"

namespace storage {

class UserStorage {
public:
    // Save user to data/users/{username}.json
    static bool save(const models::UserAccount& user);
    // Load user from data/users/{username}.json
    static std::optional<models::UserAccount> load(const std::string& username);
    // List all users from data/users/*.json
    static std::vector<models::UserAccount> listAll();
};

} // namespace storage 