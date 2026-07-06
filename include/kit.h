#pragma once

#include <string>
#include <vector>

enum class KitType {
    XACT_XJ380,
    XACT_WINDOWS,
    API_STATIC,
    XSWL
};

struct Kit {
    KitType type;
    std::string name;
    std::string version;
    std::string url;
    std::string fileName;
    std::string targetDir;
};

std::string kitTypeToString(KitType type);
KitType stringToKitType(const std::string& str);