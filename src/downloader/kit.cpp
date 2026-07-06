#include "kit.h"

std::string kitTypeToString(KitType type) {
    switch (type) {
        case KitType::XACT_XJ380: return "XACT for XJ380";
        case KitType::XACT_WINDOWS: return "XACT for Windows";
        case KitType::API_STATIC: return "API 静态库";
        case KitType::XSWL: return "XSWL";
        default: return "未知";
    }
}

KitType stringToKitType(const std::string& str) {
    if (str.find("XJ380") != std::string::npos) return KitType::XACT_XJ380;
    if (str.find("Windows") != std::string::npos) return KitType::XACT_WINDOWS;
    if (str.find("API") != std::string::npos) return KitType::API_STATIC;
    if (str.find("XSWL") != std::string::npos) return KitType::XSWL;
    return KitType::XACT_WINDOWS;
}