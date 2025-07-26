#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Types.h"
#include <filesystem>
namespace HwTool {
    class IModuleImporter {
    protected:
        static bool ValidatePath(const std::filesystem::path& path, std::vector<std::string>& errors) {
            if (!std::filesystem::exists(path)) {
                errors.push_back("File does not exist: " + path.string());
                return false;
            }
            if (!std::filesystem::is_regular_file(path)) {
                errors.push_back("Not a regular file: " + path.string());
                return false;
            }
            return true;
        }
    private:
        std::vector<std::string> m_errors;
    public:
        virtual ~IModuleImporter() = default;
        virtual std::unordered_map<std::string, Module> getModules()  = 0;
  
        bool valid() const { return m_errors.empty(); }
        std::vector<std::string> getErrors() const { return m_errors; }

    };
}