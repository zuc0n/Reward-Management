#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

class UserAccount {
public:
    std::string username;
    std::string password_hash;
    std::string email;
    bool is_admin;

    UserAccount() = default;
    UserAccount(const std::string& user, const std::string& passHash, const std::string& mail, bool admin)
        : username(user), password_hash(passHash), email(mail), is_admin(admin) {}
};

// JSON serialization
inline void to_json(nlohmann::json& j, const UserAccount& u) {
    j = nlohmann::json{
        {"username", u.username},
        {"password_hash", u.password_hash},
        {"email", u.email},
        {"is_admin", u.is_admin}
    };
}

inline void from_json(const nlohmann::json& j, UserAccount& u) {
    j.at("username").get_to(u.username);
    j.at("password_hash").get_to(u.password_hash);
    j.at("email").get_to(u.email);
    j.at("is_admin").get_to(u.is_admin);
}

} 