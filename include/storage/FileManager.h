#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace storage {

class FileManager {
public:
    // Atomically writes JSON to the given file path with exclusive lock
    static bool writeJson(const std::string& path, const nlohmann::json& j);
    // Reads JSON from the given file path with shared lock
    static bool readJson(const std::string& path, nlohmann::json& j);
};

} // namespace storage 