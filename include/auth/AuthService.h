#pragma once

#include <string>
#include <optional>

namespace auth {

class AuthService {
public:
    // Hashes a plaintext password using SHA256
    static std::string hashPassword(const std::string& password);
    // Verifies a plaintext password against a stored hash
    static bool verifyPassword(const std::string& password, const std::string& hash);

    // Initiates login by verifying credentials and generating an OTP code
    // Returns the OTP code on success, nullopt on failure
    static std::optional<std::string> initiateLogin(const std::string& username, const std::string& password);

    // Completes login by validating the OTP and issuing a session token (24h expiry)
    // Returns token string on success, nullopt on failure
    static std::optional<std::string> completeLogin(const std::string& username, const std::string& otp);

    // Validates a session token and returns associated username if valid
    static std::optional<std::string> validateToken(const std::string& token);
    // Invalidates a session token
    static bool logout(const std::string& token);
};

} // namespace auth 