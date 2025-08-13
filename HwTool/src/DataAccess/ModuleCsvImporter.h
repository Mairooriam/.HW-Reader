#include "DataAccess/IModuleImporter.h"
#include <filesystem>
#include <vector>
#include <string>
#include <set>
namespace HwTool {
    class ModuleCsvImporter : public IModuleImporter {
        std::filesystem::path m_path;
        std::vector<std::string> m_errors;
        std::set<std::string> m_namesInUse;
    public:
        ModuleCsvImporter(const std::filesystem::path& path, const std::set<std::string>& modules);
        ModuleMap getModules() override;
        ModuleMap getModules2();
    };
}