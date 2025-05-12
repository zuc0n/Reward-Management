#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

class Transaction {
public:
    std::string transaction_id;
    std::string wallet_id;
    double amount;
    std::string timestamp;
    std::string type;
    std::string description;

    Transaction() = default;
    Transaction(const std::string& id,
                const std::string& wallet,
                double amt,
                const std::string& time,
                const std::string& t,
                const std::string& desc = "")
        : transaction_id(id), wallet_id(wallet), amount(amt), timestamp(time), type(t), description(desc) {}
};

// JSON serialization
inline void to_json(nlohmann::json& j, const Transaction& t) {
    j = nlohmann::json{
        {"transaction_id", t.transaction_id},
        {"wallet_id", t.wallet_id},
        {"amount", t.amount},
        {"timestamp", t.timestamp},
        {"type", t.type},
        {"description", t.description}
    };
}

inline void from_json(const nlohmann::json& j, Transaction& t) {
    j.at("transaction_id").get_to(t.transaction_id);
    j.at("wallet_id").get_to(t.wallet_id);
    j.at("amount").get_to(t.amount);
    j.at("timestamp").get_to(t.timestamp);
    j.at("type").get_to(t.type);
    j.at("description").get_to(t.description);
}

} 