/*
 * ApiRouter.cpp
 * 
 * This file implements the API router for the Reward Management system.
 * It handles all incoming API requests, validates authentication, and routes them to the appropriate service layer.
 * The router supports user management, wallet operations, and admin functionalities.
 */

#include "api/ApiRouter.h"
#include "auth/AuthService.h"
#include "services/UserService.h"
#include "services/WalletService.h"
#include "services/AdminService.h"
#include <nlohmann/json.hpp>

namespace api {

ApiResponse ApiRouter::initiateLogin(const std::string& username, const std::string& password) {
    auto otpOpt = auth::AuthService::initiateLogin(username, password);
    if (!otpOpt) {
        return ApiResponse{false, "Invalid credentials", {}};
    }
    nlohmann::json data;
    data["otp"] = *otpOpt;
    return ApiResponse{true, "OTP generated", data};
}

ApiResponse ApiRouter::completeLogin(const std::string& username, const std::string& otp) {
    auto tokenOpt = auth::AuthService::completeLogin(username, otp);
    if (!tokenOpt) {
        return ApiResponse{false, "Invalid OTP", {}};
    }
    nlohmann::json data;
    data["token"] = *tokenOpt;
    return ApiResponse{true, "Login successful", data};
}

// User endpoints
ApiResponse ApiRouter::registerUser(const std::string& username,
                                   const std::string& password,
                                   const std::string& email) {
    bool ok = services::UserService::registerUser(username, password, email);
    if (!ok) return ApiResponse{false, "Registration failed", {}};
    return ApiResponse{true, "User registered", {}};
}

ApiResponse ApiRouter::getProfile(const std::string& token) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto profileOpt = services::UserService::getProfile(*userOpt);
    if (!profileOpt) return ApiResponse{false, "User not found", {}};
    nlohmann::json data;
    data["user"] = *profileOpt;
    return ApiResponse{true, "Profile fetched", data};
}

ApiResponse ApiRouter::updateProfile(const std::string& token,
                                    const std::string& email) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    bool ok = services::UserService::updateProfile(*userOpt, email);
    if (!ok) return ApiResponse{false, "Update failed", {}};
    return ApiResponse{true, "Profile updated", {}};
}

ApiResponse ApiRouter::changePassword(const std::string& token,
                                     const std::string& oldPassword,
                                     const std::string& newPassword) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    bool ok = services::UserService::changePassword(*userOpt, oldPassword, newPassword);
    if (!ok) return ApiResponse{false, "Change password failed", {}};
    return ApiResponse{true, "Password changed", {}};
}

ApiResponse ApiRouter::deleteUser(const std::string& token) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    bool ok = services::UserService::deleteUser(*userOpt);
    if (!ok) return ApiResponse{false, "Delete user failed", {}};
    auth::AuthService::logout(token);
    return ApiResponse{true, "User deleted", {}};
}

// Wallet endpoints
ApiResponse ApiRouter::createWallet(const std::string& token) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    
    auto walletOpt = services::WalletService::createWallet(*userOpt);
    if (!walletOpt) {
        // Check if user already has a wallet
        auto user = services::UserService::getProfile(*userOpt);
        if (user && !user->wallet_id.empty()) {
            return ApiResponse{false, "Wallet already exists for this user", {}};
        }
        return ApiResponse{false, "Failed to create wallet", {}};
    }
    
    nlohmann::json data;
    data["walletId"] = *walletOpt;
    return ApiResponse{true, "Wallet created successfully", data};
}

ApiResponse ApiRouter::getWallet(const std::string& token,
                                const std::string& walletId) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto walletOpt = services::WalletService::getWallet(walletId);
    if (!walletOpt) return ApiResponse{false, "Wallet not found", {}};
    nlohmann::json data;
    data["wallet"] = *walletOpt;
    return ApiResponse{true, "Wallet fetched", data};
}

ApiResponse ApiRouter::executeTransaction(const std::string& token,
                                        const std::string& walletId,
                                        double amount,
                                        const std::string& type,
                                        const std::string& description) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    bool ok = services::WalletService::executeTransaction(walletId, amount, type, description);
    if (!ok) return ApiResponse{false, "Transaction failed", {}};
    return ApiResponse{true, "Transaction executed", {}};
}

ApiResponse ApiRouter::getTransactions(const std::string& token,
                                      const std::string& walletId) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto list = services::WalletService::getTransactions(walletId);
    nlohmann::json data;
    data["transactions"] = list;
    return ApiResponse{true, "Transactions fetched", data};
}

// Admin endpoints
ApiResponse ApiRouter::listUsers(const std::string& token) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto profileOpt = services::UserService::getProfile(*userOpt);
    if (!profileOpt || !profileOpt->is_admin) return ApiResponse{false, "Unauthorized", {}};
    auto users = services::AdminService::listAllUsers();
    nlohmann::json data;
    data["users"] = users;
    return ApiResponse{true, "Users fetched", data};
}

ApiResponse ApiRouter::adminCreateUser(const std::string& token,
                                       const std::string& username,
                                       const std::string& password,
                                       const std::string& email,
                                       bool isAdmin) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto profileOpt = services::UserService::getProfile(*userOpt);
    if (!profileOpt || !profileOpt->is_admin) return ApiResponse{false, "Unauthorized", {}};
    bool ok = services::AdminService::createUser(username, password, email, isAdmin);
    if (!ok) return ApiResponse{false, "Admin create user failed", {}};
    return ApiResponse{true, "User created by admin", {}};
}

ApiResponse ApiRouter::adminUpdateUser(const std::string& token,
                                       const std::string& username,
                                       const std::string& email,
                                       bool isAdmin) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto profileOpt = services::UserService::getProfile(*userOpt);
    if (!profileOpt || !profileOpt->is_admin) return ApiResponse{false, "Unauthorized", {}};
    bool ok = services::AdminService::updateUser(username, email, isAdmin);
    if (!ok) return ApiResponse{false, "Admin update user failed", {}};
    return ApiResponse{true, "User updated by admin", {}};
}

ApiResponse ApiRouter::adminResetPassword(const std::string& token,
                                          const std::string& username,
                                          const std::string& newPassword) {
    auto userOpt = auth::AuthService::validateToken(token);
    if (!userOpt) return ApiResponse{false, "Authentication failed", {}};
    auto profileOpt = services::UserService::getProfile(*userOpt);
    if (!profileOpt || !profileOpt->is_admin) return ApiResponse{false, "Unauthorized", {}};
    bool ok = services::AdminService::resetPassword(username, newPassword);
    if (!ok) return ApiResponse{false, "Admin reset password failed", {}};
    return ApiResponse{true, "Password reset by admin", {}};
}

} // namespace api 