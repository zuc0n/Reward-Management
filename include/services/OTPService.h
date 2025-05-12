#pragma once

#include <string>

namespace services {

class OTPService {
public:
    // Generates a 6-digit OTP for the user, stores it with 5min expiry
    static std::string generateOTP(const std::string& username);

    // Validates the OTP for the user; returns true if correct and not expired. Invalidates OTP on success.
    static bool validateOTP(const std::string& username, const std::string& code);
};

} // namespace services 