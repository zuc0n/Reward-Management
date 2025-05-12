#pragma once

#include <string>
#include <optional>
#include "models/UserAccount.h"

namespace services {

class UserService {
public:
    // Registers a new user; returns true on success (user didn't exist before)
    static bool registerUser(const std::string& username,
                             const std::string& password,
                             const std::string& email,
                             bool isAdmin = false);

    // Retrieves user profile if exists
    static std::optional<models::UserAccount> getProfile(const std::string& username);

    // Updates user email; returns true on success
    static bool updateProfile(const std::string& username,
                              const std::string& email);

    // Changes password if oldPassword matches; returns true on success
    static bool changePassword(const std::string& username,
                               const std::string& oldPassword,
                               const std::string& newPassword);

    // Deletes the user account; returns true on success
    static bool deleteUser(const std::string& username);
};

} // namespace services 