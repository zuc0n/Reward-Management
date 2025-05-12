#include "services/AdminService.h"
#include "storage/UserStorage.h"
#include "auth/AuthService.h"
#include "services/UserService.h"

#include <vector>

namespace services {

std::vector<models::UserAccount> AdminService::listAllUsers() {
    return storage::UserStorage::listAll();
}

bool AdminService::createUser(const std::string& username,
                              const std::string& password,
                              const std::string& email,
                              bool isAdmin) {
    // Directly register user (bypass OTP)
    return UserService::registerUser(username, password, email, isAdmin);
}

bool AdminService::updateUser(const std::string& username,
                              const std::string& email,
                              bool isAdmin) {
    auto userOpt = storage::UserStorage::load(username);
    if (!userOpt) return false;
    auto user = *userOpt;
    user.email = email;
    user.is_admin = isAdmin;
    return storage::UserStorage::save(user);
}

bool AdminService::resetPassword(const std::string& username,
                                 const std::string& newPassword) {
    auto userOpt = storage::UserStorage::load(username);
    if (!userOpt) return false;
    auto user = *userOpt;
    user.password_hash = auth::AuthService::hashPassword(newPassword);
    return storage::UserStorage::save(user);
}

} // namespace services 