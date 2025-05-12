/*
 * AuthService.cpp
 * 
 * This file implements the authentication service for the Reward Management system.
 * It handles password hashing, OTP generation, session token management, and login/logout operations.
 */

#include "auth/AuthService.h"
#include "storage/FileManager.h"
#include "storage/UserStorage.h"
#include "services/OTPService.h"

#include <nlohmann/json.hpp>
#include <openssl/sha.h>

#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <system_error>

namespace auth {

std::string AuthService::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::setw(2) << static_cast<int>(hash[i]);
    }
    return oss.str();
}

bool AuthService::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}

std::optional<std::string> AuthService::initiateLogin(const std::string& username, const std::string& password) {
    auto userOpt = storage::UserStorage::load(username);
    if (!userOpt) return std::nullopt;
    const auto& user = *userOpt;
    if (!verifyPassword(password, user.password_hash)) return std::nullopt;

    // Generate and store OTP
    return services::OTPService::generateOTP(username);
}

std::optional<std::string> AuthService::completeLogin(const std::string& username, const std::string& otp) {
    if (!services::OTPService::validateOTP(username, otp)) return std::nullopt;

    // Generate session token
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint64_t> distr;
    uint64_t rnd = distr(eng);
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream tokenStream;
    tokenStream << std::hex << rnd << now;
    std::string token = tokenStream.str();

    // Prepare session JSON
    nlohmann::json j;
    j["username"] = username;
    auto expiry = std::chrono::system_clock::now() + std::chrono::hours(24);
    auto expiry_ts = std::chrono::duration_cast<std::chrono::seconds>(expiry.time_since_epoch()).count();
    j["expiry"] = expiry_ts;

    std::string path = "data/sessions/" + token + ".json";
    if (!storage::FileManager::writeJson(path, j)) {
        return std::nullopt;
    }
    return token;
}

std::optional<std::string> AuthService::validateToken(const std::string& token) {
    std::string path = "data/sessions/" + token + ".json";
    nlohmann::json j;
    if (!storage::FileManager::readJson(path, j)) return std::nullopt;
    try {
        long long expiry_ts = j.at("expiry").get<long long>();
        long long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        if (now > expiry_ts) {
            std::error_code ec;
            std::filesystem::remove(path, ec);
            return std::nullopt;
        }
        return j.at("username").get<std::string>();
    } catch (...) {
        return std::nullopt;
    }
}

bool AuthService::logout(const std::string& token) {
    std::string path = "data/sessions/" + token + ".json";
    std::error_code ec;
    return std::filesystem::remove(path, ec);
}

} // namespace auth 