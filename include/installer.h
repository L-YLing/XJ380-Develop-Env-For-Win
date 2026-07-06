#pragma once

#include "kit.h"
#include <vector>
#include <string>

class Installer {
public:
    Installer(const std::string& baseDir);
    
    bool installKit(const Kit& kit, const std::string& downloadPath);
    bool installFromOffline(const Kit& kit);
    bool extractZip(const std::string& zipPath, const std::string& destDir);
    bool extractZipWithProgress(const std::string& zipPath, const std::string& destDir);
    bool copyDirectory(const std::string& source, const std::string& target);
    
private:
    std::string baseDir_;
    
    bool createAssociations();
    bool setupEnvironment();
};