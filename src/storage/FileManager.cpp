#include "storage/FileManager.h"
#include <fstream>
#include <filesystem>
#include <system_error>

namespace storage {
namespace fs = std::filesystem;

bool FileManager::writeJson(const std::string& path, const nlohmann::json& j) {
    fs::path p(path);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }
    std::string tmpPath = path + ".tmp";
    {
        std::ofstream ofs(tmpPath, std::ios::trunc);
        if (!ofs) return false;
        ofs << j.dump(4);
        ofs.flush();
        if (!ofs) return false;
    }
    std::error_code ec;
    fs::rename(tmpPath, path, ec);
    return !ec;
}

bool FileManager::readJson(const std::string& path, nlohmann::json& j) {
    std::ifstream ifs(path);
    if (!ifs) return false;
    try {
        ifs >> j;
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace storage 