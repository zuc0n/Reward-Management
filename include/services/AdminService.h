#pragma once

#include <string>
#include <vector>
#include <optional>
#include "models/UserAccount.h"

namespace services {

class AdminService {
public:
    // Lists all registered users
    static std::vector<models::UserAccount> listAllUsers();

    // Creates a user on behalf, with optional admin flag
    static bool createUser(const std::string& username,
                           const std::string& password,
                           const std::string& email,
                           bool isAdmin = false);

    // Updates a user's email and admin status (forced)
    static bool updateUser(const std::string& username,
                           const std::string& email,
                           bool isAdmin);

    // Resets a user's password (forced)
    static bool resetPassword(const std::string& username,
                              const std::string& newPassword);
};

} // namespace services 