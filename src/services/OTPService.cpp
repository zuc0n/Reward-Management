#include "services/OTPService.h"
#include "storage/FileManager.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <random>
#include <filesystem>
#include <system_error>
#include <sstream>
#include <iomanip>

namespace services {
namespace fs = std::filesystem;

std::string OTPService::generateOTP(const std::string& username) {
    // Generate 6-digit code
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 999999);
    int codeNum = dist(gen);
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << codeNum;
    std::string code = oss.str();

    // Prepare JSON with expiry (5 minutes)
    nlohmann::json j;
    j["code"] = code;
    auto expires = std::chrono::system_clock::now() + std::chrono::minutes(5);
    auto exp_ts = std::chrono::duration_cast<std::chrono::seconds>(expires.time_since_epoch()).count();
    j["expiry"] = exp_ts;

    // Write to file
    std::string path = "data/otps/" + username + ".json";
    // ensure directory exists
    if (fs::path(path).has_parent_path()) {
        std::error_code ec;
        fs::create_directories(fs::path(path).parent_path(), ec);
    }
    storage::FileManager::writeJson(path, j);
    return code;
}

bool OTPService::validateOTP(const std::string& username, const std::string& code) {
    std::string path = "data/otps/" + username + ".json";
    nlohmann::json j;
    if (!storage::FileManager::readJson(path, j)) {
        return false;
    }
    try {
        std::string stored = j.at("code").get<std::string>();
        long long exp_ts = j.at("expiry").get<long long>();
        long long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        if (now > exp_ts || code != stored) {
            return false;
        }
        // Invalidate after successful validation
        std::error_code ec;
        fs::remove(path, ec);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace services 