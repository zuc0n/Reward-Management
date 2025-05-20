#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace client {

class CLIClient {
public:
    // Starts the interactive CLI loop
    void run();
    void handleUserMenu(const std::string& token);
    void handleAdminMenu(const std::string& token);
    void showProfile(const std::string& token);
};

} // namespace client 