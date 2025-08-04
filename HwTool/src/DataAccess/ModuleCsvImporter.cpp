#include "ModuleCsvImporter.h"

#include <fastCsv/csv.h>

#include <fstream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

#include "HardwareBuilder.h"

namespace HwTool {
    ModuleCsvImporter::ModuleCsvImporter(const std::filesystem::path& path, const std::set<std::string>& modules)
    : m_path{path}, m_namesInUse{modules} {
        this->ValidatePath(path, m_errors);
    }

    ModuleMap ModuleCsvImporter::getModules() {
        io::CSVReader<3> in(m_path.string().c_str());
        in.read_header(io::ignore_extra_column, "Location", "Name", "Type");
        std::string location, name, type;
        ModuleMap modules;
        HardwareBuilder hwb;
        std::string previousBase;
        while (in.read_row(location, name, type)) {
            ModulePack mp = hwb.createCard(
            name, magic_enum::enum_cast<cardType>(type).value_or(cardType::ERROR), m_namesInUse);
            
            if (!previousBase.empty())
                mp.base.connections[0].targetModuleName = previousBase;
            
            modules[mp.base.name] = mp.base;
            modules[mp.card.name] = mp.card;
            modules[mp.tb.name] = mp.tb;

            previousBase = mp.base.name;
            m_namesInUse.insert(mp.base.name);
            m_namesInUse.insert(mp.card.name);
            m_namesInUse.insert(mp.tb.name);
            //printf("vendor %s\n", vendor.c_str());
        }
        return modules;
    }
}  // namespace HwTool
