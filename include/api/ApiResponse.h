#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace api {

struct ApiResponse {
    bool success;
    std::string message;
    nlohmann::json data;
};

} // namespace api 