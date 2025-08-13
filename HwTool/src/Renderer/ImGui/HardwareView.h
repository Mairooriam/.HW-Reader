#pragma once
#include "RenderData.h"
#include <string>
#include <functional>

namespace Mir {
    class HardwareView {
    public:
        using ModuleFilter = std::function<bool(const HwTool::Module&)>;
        
        enum class FilterType {
            All, CardsOnly, BasesOnly, CpusOnly, 
            CpuBasesOnly, ByType, ByName
        };
        
        struct CacheOption {
            std::string name;
            const HwTool::ModuleMap* moduleMap;
        };

        std::vector<CacheOption> m_caches;

    private:
        struct State {
            ModuleFilter filter;
            FilterType filterType = FilterType::All;
            std::string filterName = "All";
            HwTool::cardType selectedCardType = HwTool::cardType::ERROR;
            std::string namePattern = "";
            int selectedCacheIndex = 0;
        } m_state;
        
    public:
        HardwareView();
        void setFilter(ModuleFilter filter, FilterType filterType, const std::string& filterName);
        void render(const std::string& windowName, const HwTool::ModuleMap& moduleMap, 
                   std::string& selectedCard, const HwTool::ModuleMap* baseCardCache = nullptr);
        
        void renderFilterSelection();
        void renderCacheSelection();
        void setCaches(const std::vector<CacheOption>& caches);
        static ModuleFilter showAll();
        static ModuleFilter showCardsOnly();
        static ModuleFilter showBasesOnly();
        static ModuleFilter showCpusOnly();
        static ModuleFilter showCpuBasesOnly();
        static ModuleFilter showByType(HwTool::cardType type);
        static ModuleFilter showByName(const std::string& namePattern);
    };
}