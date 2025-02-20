#include "version.h"
#include "helpers/logging.h"
#include <fstream>

namespace Engine {
    const std::string VERSION = []() {
        std::ifstream versionFile("version.txt");
        // IF we can't read the version file, return "v0.0.0 (unknown-unknown)"
        if (!versionFile.is_open()) return std::string("v0.0.0 (unknown-unknown)");

        std::string commitHash, branch, versionNum;
        std::getline(versionFile, commitHash);
        std::getline(versionFile, branch);
        std::getline(versionFile, versionNum);
        std::string response = "v" + versionNum + " (" + branch + "-" + commitHash + ")";
        return response;
    }();

    const std::string VERSION_NUMBER = []() {
        std::ifstream versionFile("version.txt");
        // IF we can't read the version file, return 0
        if (!versionFile.is_open()) return std::string("INVALID_VERSION");

        std::string commitHash, branch, versionNum;
        std::getline(versionFile, commitHash);
        std::getline(versionFile, branch);
        std::getline(versionFile, versionNum);

        std::string response = versionNum;
        return response;
    }();
}
