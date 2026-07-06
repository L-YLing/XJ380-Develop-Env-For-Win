#pragma once

#include "kit.h"
#include <vector>
#include <memory>

enum class SourceType {
    OFFICIAL,
    GITHUB,
    OFFLINE
};

class ISource {
public:
    virtual ~ISource() = default;
    virtual SourceType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<Kit> getKits() = 0;
    virtual bool isAvailable() = 0;
};

class SourceFactory {
public:
    static std::unique_ptr<ISource> createSource(SourceType type);
};

std::string sourceTypeToString(SourceType type);
SourceType stringToSourceType(const std::string& str);