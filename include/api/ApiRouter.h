#pragma once

#include "ApiResponse.h"
#include <string>
#include <nlohmann/json.hpp>

namespace api {

class ApiRouter {
public:
    // Auth endpoints
    // Step 1: verify credentials and generate OTP
    static ApiResponse initiateLogin(const std::string& username, const std::string& password);

    // Step 2: validate OTP and issue session token
    static ApiResponse completeLogin(const std::string& username, const std::string& otp);

    // User endpoints
    static ApiResponse registerUser(const std::string& username,
                                    const std::string& password,
                                    const std::string& email);
    static ApiResponse getProfile(const std::string& token);
    static ApiResponse updateProfile(const std::string& token,
                                     const std::string& email);
    static ApiResponse changePassword(const std::string& token,
                                      const std::string& oldPassword,
                                      const std::string& newPassword);
    static ApiResponse deleteUser(const std::string& token);

    // Wallet endpoints
    static ApiResponse createWallet(const std::string& token);
    static ApiResponse getWallet(const std::string& token,
                                 const std::string& walletId);
    static ApiResponse executeTransaction(const std::string& token,
                                         const std::string& walletId,
                                         double amount,
                                         const std::string& type,
                                         const std::string& description);
    static ApiResponse getTransactions(const std::string& token,
                                       const std::string& walletId);

    // Admin endpoints
    static ApiResponse listUsers(const std::string& token);
    static ApiResponse adminCreateUser(const std::string& token,
                                       const std::string& username,
                                       const std::string& password,
                                       const std::string& email,
                                       bool isAdmin);
    static ApiResponse adminUpdateUser(const std::string& token,
                                       const std::string& username,
                                       const std::string& email,
                                       bool isAdmin);
    static ApiResponse adminResetPassword(const std::string& token,
                                          const std::string& username,
                                          const std::string& newPassword);
};

} // namespace api 