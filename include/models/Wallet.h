#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace models {

class Wallet {
public:
    std::string wallet_id;
    std::string owner_username;
    double balance;
    std::vector<std::string> transaction_ids;

    Wallet() = default;
    Wallet(const std::string& id, const std::string& owner, double bal)
        : wallet_id(id), owner_username(owner), balance(bal) {}
};

// JSON serialization
inline void to_json(nlohmann::json& j, const Wallet& w) {
    j = nlohmann::json{
        {"wallet_id", w.wallet_id},
        {"owner_username", w.owner_username},
        {"balance", w.balance},
        {"transaction_ids", w.transaction_ids}
    };
}

inline void from_json(const nlohmann::json& j, Wallet& w) {
    j.at("wallet_id").get_to(w.wallet_id);
    j.at("owner_username").get_to(w.owner_username);
    j.at("balance").get_to(w.balance);
    j.at("transaction_ids").get_to(w.transaction_ids);
}

} 