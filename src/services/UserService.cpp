#include "services/UserService.h"
#include "storage/UserStorage.h"
#include "auth/AuthService.h"

#include <filesystem>
#include <system_error>

namespace services {

bool UserService::registerUser(const std::string& username,
                               const std::string& password,
                               const std::string& email,
                               bool isAdmin) {
    // Check if user exists
    if (storage::UserStorage::load(username).has_value()) return false;
    // Hash password
    std::string passHash = auth::AuthService::hashPassword(password);
    models::UserAccount user(username, passHash, email, isAdmin);
    return storage::UserStorage::save(user);
}

std::optional<models::UserAccount> UserService::getProfile(const std::string& username) {
    return storage::UserStorage::load(username);
}

bool UserService::updateProfile(const std::string& username,
                                const std::string& email) {
    auto userOpt = storage::UserStorage::load(username);
    if (!userOpt) return false;
    auto user = *userOpt;
    user.email = email;
    return storage::UserStorage::save(user);
}

bool UserService::changePassword(const std::string& username,
                                 const std::string& oldPassword,
                                 const std::string& newPassword) {
    auto userOpt = storage::UserStorage::load(username);
    if (!userOpt) return false;
    auto user = *userOpt;
    if (!auth::AuthService::verifyPassword(oldPassword, user.password_hash)) {
        return false;
    }
    user.password_hash = auth::AuthService::hashPassword(newPassword);
    return storage::UserStorage::save(user);
}

bool UserService::deleteUser(const std::string& username) {
    std::filesystem::path path("data/users/" + username + ".json");
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

} // namespace services 