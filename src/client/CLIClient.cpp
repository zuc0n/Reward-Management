/*
 * CLIClient.cpp
 * 
 * This file implements the command-line interface (CLI) for the Reward Management system.
 * It provides a user-friendly menu for interacting with the system, including user management, wallet operations, and admin functionalities.
 */

#include "client/CLIClient.h"
#include "api/ApiRouter.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>  // for std::transform in CLI validation
#include <random>

namespace client {

void CLIClient::run() {
    std::string token;
    bool isAdmin = false;
    bool exitApp = false;

    while (!exitApp) {
        if (token.empty()) {
            std::cout << "\n=== Welcome ===\n";
            std::cout << "1) Login\n2) Register\n0) Exit\nChoice: ";
            int choice;
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid selection\n";
                continue;
            }
            if (choice == 1) {
                std::string username, password;
                std::cout << "Username: "; std::cin >> username;
                std::cout << "Password: "; std::cin >> password;
                if (username.empty() || password.empty()) {
                    std::cout << "Username and password cannot be empty\n";
                    continue;
                }
                auto res1 = api::ApiRouter::initiateLogin(username, password);
                if (!res1.success) {
                    std::cout << "Error: " << res1.message << "\n";
                    continue;
                }
                std::string otp = res1.data["otp"];
                std::cout << "OTP: " << otp << "\n";
                std::cout << "Enter OTP: ";
                std::string code;
                std::cin >> code;
                auto res2 = api::ApiRouter::completeLogin(username, code);
                if (!res2.success) {
                    std::cout << "Error: " << res2.message << "\n";
                } else {
                    token = res2.data["token"];
                    // determine admin status
                    auto prof = api::ApiRouter::getProfile(token);
                    if (prof.success) {
                        isAdmin = prof.data["user"]["is_admin"].get<bool>();
                    }
                    std::cout << "Login successful!\n";
                }
            } else if (choice == 2) {
                std::string username, password, email;
                std::cout << "Choose username: "; std::cin >> username;
                if (username.empty()) {
                    std::cout << "Username cannot be empty\n";
                    continue;
                }
                std::cout << "Choose password: "; std::cin >> password;
                if (password.empty()) {
                    std::cout << "Password cannot be empty\n";
                    continue;
                }
                std::cout << "Email: "; std::cin >> email;
                if (email.empty()) {
                    std::cout << "Email cannot be empty\n";
                    continue;
                }
                auto res = api::ApiRouter::registerUser(username, password, email);
                std::cout << (res.success ? res.message : std::string("Error: ") + res.message) << "\n";
            } else if (choice == 0) {
                exitApp = true;
            } else {
                std::cout << "Invalid choice\n";
            }
        } else {
            std::cout << "\n=== Main Menu ===\n";
            std::cout << "1) Show Profile\n";
            std::cout << "2) Update Email\n";
            std::cout << "3) Change Password\n";
            std::cout << "4) Delete Account\n";
            std::cout << "5) Create Wallet\n";
            std::cout << "6) Show Wallet\n";
            std::cout << "7) Execute Transaction\n";
            std::cout << "8) List Transactions\n";
            std::cout << "9) Logout\n";
            if (isAdmin) {
                std::cout << "10) List Users\n";
                std::cout << "11) Create User (admin)\n";
                std::cout << "12) Update User (admin)\n";
                std::cout << "13) Reset Password (admin)\n";
            }
            std::cout << "0) Exit\nChoice: ";
            int choice;
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid selection\n";
                continue;
            }
            switch (choice) {
                case 1: {
                    showProfile(token);
                    break;
                }
                case 2: {
                    std::string email;
                    std::cout << "New email: "; std::cin >> email;
                    auto res = api::ApiRouter::updateProfile(token, email);
                    std::cout << res.message << "\n";
                    break;
                }
                case 3: {
                    std::string oldp, newp;
                    std::cout << "Old password: "; std::cin >> oldp;
                    std::cout << "New password: "; std::cin >> newp;
                    auto res = api::ApiRouter::changePassword(token, oldp, newp);
                    std::cout << res.message << "\n";
                    break;
                }
                case 4: {
                    std::cout << "Are you sure you want to delete your account? (y/n): ";
                    char c; std::cin >> c;
                    if (c == 'y' || c == 'Y') {
                        auto res = api::ApiRouter::deleteUser(token);
                        std::cout << res.message << "\n";
                        token.clear();
                    }
                    break;
                }
                case 5: {
                    auto res = api::ApiRouter::createWallet(token);
                    if (res.success) {
                        std::cout << "Wallet ID: " << res.data["walletId"] << "\n";
                    } else {
                        std::cout << "Error: " << res.message << "\n";
                    }
                    break;
                }
                case 6: {
                    std::string walletId;
                    std::cout << "Wallet ID: "; std::cin >> walletId;
                    if (walletId.empty()) {
                        std::cout << "Wallet ID cannot be empty\n";
                        break;
                    }
                    auto res = api::ApiRouter::getWallet(token, walletId);
                    if (!res.success) {
                        std::cout << "Error: " << res.message << "\n";
                    } else {
                        auto w = res.data["wallet"];
                        std::cout << "Wallet ID: " << w["wallet_id"] << "\n";
                        std::cout << "Owner: " << w["owner_username"] << "\n";
                        std::cout << "Balance: " << w["balance"] << "\n";
                        std::cout << "Transactions: ";
                        for (auto &id : w["transaction_ids"]) std::cout << id.get<std::string>() << " ";
                        std::cout << "\n";
                    }
                    break;
                }
                case 7: {
                    std::string senderWalletId, recipientWalletId, type, desc;
                    double amount;
                    
                    // First ask for transaction type
                    std::cout << "Type (credit/debit): "; std::cin >> type;
                    // normalize
                    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
                    if (type != "credit" && type != "debit") {
                        std::cout << "Type must be 'credit' or 'debit'\n";
                        break;
                    }

                    // Get sender's wallet ID first
                    auto profileRes = api::ApiRouter::getProfile(token);
                    if (!profileRes.success || profileRes.data["user"]["wallet_id"].get<std::string>().empty()) {
                        std::cout << "Error: You don't have a wallet yet\n";
                        break;
                    }
                    senderWalletId = profileRes.data["user"]["wallet_id"].get<std::string>();

                    // For debit transactions, ask for recipient's wallet ID
                    if (type == "debit") {
                        std::cout << "Recipient's Wallet ID: "; std::cin >> recipientWalletId;
                        if (recipientWalletId.empty()) {
                            std::cout << "Wallet ID cannot be empty\n";
                            break;
                        }
                        // Check if recipient's wallet exists
                        auto walletCheck = api::ApiRouter::getWallet(token, recipientWalletId);
                        if (!walletCheck.success) {
                            std::cout << "Error: Recipient's wallet not found\n";
                            break;
                        }
                        std::cout << "Recipient's wallet found. Owner: " 
                                 << walletCheck.data["wallet"]["owner_username"].get<std::string>() << "\n";
                    }

                    std::cout << "Amount: "; std::cin >> amount;
                    if (!std::cin || amount <= 0) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Amount must be a positive number\n";
                        break;
                    }

                    std::cout << "Description: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, desc);

                    // Generate and verify OTP before proceeding with transaction
                    std::string username = profileRes.data["user"]["username"].get<std::string>();
                    
                    // Generate a simple 6-digit OTP
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, 999999);
                    std::string otp = std::to_string(dis(gen));
                    // Pad with leading zeros if necessary
                    otp = std::string(6 - otp.length(), '0') + otp;
                    
                    std::cout << "OTP for transaction verification: " << otp << "\n";
                    std::cout << "Enter OTP to confirm transaction: ";
                    std::string enteredOtp;
                    std::cin >> enteredOtp;
                    
                    if (enteredOtp != otp) {
                        std::cout << "Error: Invalid OTP. Transaction cancelled.\n";
                        break;
                    }

                    // If OTP verification successful, proceed with transaction
                    // For debit transactions, use sender's wallet
                    // For credit transactions, also use sender's wallet (since it's a credit to their own wallet)
                    auto res = api::ApiRouter::executeTransaction(token, senderWalletId, amount, type, desc);
                    if (res.success && type == "debit") {
                        // If debit was successful, credit the recipient's wallet
                        auto creditRes = api::ApiRouter::executeTransaction(token, recipientWalletId, amount, "credit", 
                            "Received from " + username);
                        if (!creditRes.success) {
                            std::cout << "Warning: Money was deducted but transfer to recipient failed\n";
                        }
                    }
                    std::cout << res.message << "\n";
                    break;
                }
                case 8: {
                    std::string walletId;
                    std::cout << "Wallet ID: "; std::cin >> walletId;
                    auto res = api::ApiRouter::getTransactions(token, walletId);
                    if (!res.success) {
                        std::cout << "Error: " << res.message << "\n";
                    } else {
                        for (auto &j : res.data["transactions"]) {
                            std::cout << "ID: " << j["transaction_id"]
                                      << " | Amount: " << j["amount"]
                                      << " | Type: " << j["type"]
                                      << " | Time: " << j["timestamp"]
                                      << " | Desc: " << j["description"] << "\n";
                        }
                    }
                    break;
                }
                case 9: {
                    token.clear();
                    std::cout << "Logged out\n";
                    break;
                }
                case 10: {
                    if (!isAdmin) { std::cout << "Invalid choice\n"; break; }
                    auto res = api::ApiRouter::listUsers(token);
                    if (!res.success) {
                        std::cout << "Error: " << res.message << "\n";
                    } else {
                        for (auto &u : res.data["users"]) {
                            std::cout << u["username"] << " | "
                                      << u["email"] << " | admin="
                                      << (u["is_admin"].get<bool>() ? "yes" : "no")
                                      << "\n";
                        }
                    }
                    break;
                }
                case 11: {
                    if (!isAdmin) { std::cout << "Invalid choice\n"; break; }
                    std::string username, password, email;
                    char adm;
                    std::cout << "New username: "; std::cin >> username;
                    std::cout << "New password: "; std::cin >> password;
                    std::cout << "Email: "; std::cin >> email;
                    std::cout << "Is admin? (y/n): "; std::cin >> adm;
                    bool na = (adm=='y'||adm=='Y');
                    auto res = api::ApiRouter::adminCreateUser(token, username, password, email, na);
                    std::cout << (res.success ? res.message : std::string("Error: ") + res.message) << "\n";
                    break;
                }
                case 12: {
                    if (!isAdmin) { std::cout << "Invalid choice\n"; break; }
                    std::string username, email;
                    char adm;
                    std::cout << "Username to update: "; std::cin >> username;
                    std::cout << "New email: "; std::cin >> email;
                    std::cout << "Is admin? (y/n): "; std::cin >> adm;
                    bool na = (adm=='y'||adm=='Y');
                    auto res = api::ApiRouter::adminUpdateUser(token, username, email, na);
                    std::cout << (res.success ? res.message : std::string("Error: ") + res.message) << "\n";
                    break;
                }
                case 13: {
                    if (!isAdmin) { std::cout << "Invalid choice\n"; break; }
                    std::string username, newPass;
                    std::cout << "Username to reset: "; std::cin >> username;
                    std::cout << "New password: "; std::cin >> newPass;
                    auto res = api::ApiRouter::adminResetPassword(token, username, newPass);
                    std::cout << (res.success ? res.message : std::string("Error: ") + res.message) << "\n";
                    break;
                }
                case 0: {
                    exitApp = true;
                    break;
                }
                default:
                    std::cout << "Invalid choice\n";
            }
        }
    }
}

void CLIClient::showProfile(const std::string& token) {
    auto res = api::ApiRouter::getProfile(token);
    if (!res.success) {
        std::cout << "Error: " << res.message << "\n";
        return;
    }
    auto user = res.data["user"];
    std::cout << "Username: \"" << user["username"].get<std::string>() << "\"\n";
    std::cout << "Email: \"" << user["email"].get<std::string>() << "\"\n";
    std::cout << "Admin: " << (user["is_admin"].get<bool>() ? "Yes" : "No") << "\n";
    if (!user["wallet_id"].get<std::string>().empty()) {
        std::cout << "Wallet ID: \"" << user["wallet_id"].get<std::string>() << "\"\n";
    } else {
        std::cout << "Wallet: Not created yet\n";
    }
}

} // namespace client 