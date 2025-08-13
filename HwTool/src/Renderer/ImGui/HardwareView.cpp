#include "HardwareView.h"
#include "imgui.h"
#include <format>
#include <functional>

namespace {
    void renderCard(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache, std::string& selectedCard);
    void renderCardBody(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache);
    void renderCardHover(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache);
    void renderConnections(const HwTool::Module& m);
    void renderParameters(const HwTool::Module& m);
    void renderConnector(const HwTool::Module& m);
    void renderGroup(const HwTool::Module& m);
    
    void renderCard(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache, std::string& selectedCard) {
        std::string name = "Name: " + m.name + "\t Type: " + std::string(magic_enum::enum_name<HwTool::cardType>(m.type)) + "\t version: " + m.version + "\n";
        bool treeOpen = ImGui::TreeNode(name.c_str());
        
        if (ImGui::IsItemActive()) {
            selectedCard = m.name;
        }
        
        renderCardHover(m, baseCardCache);
        
        if (treeOpen) {
            renderCardBody(m, baseCardCache);
            ImGui::TreePop();
        }
    }
    
    void renderCardBody(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache) {
        renderConnections(m);
        renderParameters(m);
        renderConnector(m);
        renderGroup(m);
    }
    
    void renderConnections(const HwTool::Module& m) {
        if (!m.connections.empty() && ImGui::TreeNode("Connections")) {
            for (size_t i = 0; i < m.connections.size(); ++i) {
                const auto& conn = m.connections[i];
                std::string connLabel = "Connection " + std::to_string(i);
                if (ImGui::TreeNode(connLabel.c_str())) {
                    ImGui::Text("Connector: %s", magic_enum::enum_name(conn.connector).data());
                    ImGui::Text("Target Module: %s", conn.targetModuleName.c_str());
                    ImGui::Text("Target Connector: %s", magic_enum::enum_name(conn.targetConnector).data());
                    if (conn.targetModule) {
                        ImGui::Text("Target Module Ptr: Valid");
                    } else {
                        ImGui::Text("Target Module Ptr: nullptr");
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    
    void renderParameters(const HwTool::Module& m) {
        if (!m.parameters.empty() && ImGui::TreeNode("Parameters")) {
            for (size_t i = 0; i < m.parameters.size(); ++i) {
                const auto& param = m.parameters[i];
                ImGui::Text("ID: %s, Value: %s", param.id.c_str(), param.value.c_str());
            }
            ImGui::TreePop();
        }
    }
    
    void renderConnector(const HwTool::Module& m) {
        if (!m.connector.name.empty() && ImGui::TreeNode("Connector")) {
            ImGui::Text("Name: %s", m.connector.name.c_str());
            if (!m.connector.parameters.empty()) {
                if (ImGui::TreeNode("Connector Parameters")) {
                    for (const auto& param : m.connector.parameters) {
                        ImGui::Text("ID: %s, Value: %s", param.id.c_str(), param.value.c_str());
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
    
    void renderGroup(const HwTool::Module& m) {
        if (!m.group.id.empty() && ImGui::TreeNode("Group")) {
            ImGui::Text("ID: %s", m.group.id.c_str());
            ImGui::TreePop();
        }
    }

    void renderCardHover(const HwTool::Module& m, const HwTool::ModuleMap* baseCardCache) {
        if (ImGui::IsItemHovered()) {
            static float hoverTime = 0.0f;
            static const void* lastHoveredItem = nullptr;
            const void* currentItem = &m; 
            
            if (lastHoveredItem != currentItem) {
                hoverTime = 0.0f; 
                lastHoveredItem = currentItem;
            }
            
            hoverTime += ImGui::GetIO().DeltaTime;
            
            if (hoverTime > 0.5f) { 
                ImGui::BeginTooltip();
                for (size_t i = 0; i < m.connections.size(); ++i) {
                    const auto& con = m.connections[i];
                    std::string nameOfTarget = "";
                    std::string text = "";
                    if (con.connector == HwTool::ConnectorType::SL && baseCardCache) {
                        nameOfTarget = HwTool::Utils::getBaseCard(con.targetModuleName, *baseCardCache);
                        text = std::format("Target_{}: {} [{}]", i + 1, con.targetModuleName, nameOfTarget);
                    } else {
                        text = std::format("Target_{}: {} ", i + 1, con.targetModuleName);
                    }
                    
                    ImGui::Text("%s", text.c_str());
                }
                ImGui::EndTooltip();
            }
        } else {
            static float hoverTime = 0.0f;
            static const void* lastHoveredItem = nullptr;
            hoverTime = 0.0f; 
            lastHoveredItem = nullptr;
        }
    }
}

namespace Mir {

    HardwareView::HardwareView() {
        m_state.filter = showAll();
        m_state.filterType = FilterType::All;
        m_state.filterName = "All";
        m_state.selectedCardType = HwTool::cardType::ERROR;
        m_state.namePattern = "";
        m_state.selectedCacheIndex = 0;
    }
    void HardwareView::setCaches(const std::vector<CacheOption>& caches) {
        m_caches = caches;
        if (m_state.selectedCacheIndex >= m_caches.size()) {
            m_state.selectedCacheIndex = 0;
        }
    }
    void HardwareView::setFilter(ModuleFilter filter, FilterType filterType, const std::string& filterName) {
        m_state.filter = filter;
        m_state.filterType = filterType;
        m_state.filterName = filterName;
    }

    void HardwareView::render(const std::string& windowName, const HwTool::ModuleMap& moduleMap, 
                             std::string& selectedCard, const HwTool::ModuleMap* baseCardCache) {
        ImGui::Begin(windowName.c_str());
        
        if (!m_caches.empty()) {
            renderCacheSelection();
            ImGui::Separator();
            
            renderFilterSelection();
            ImGui::Separator();
            
            const HwTool::ModuleMap* currentModuleMap = m_caches[m_state.selectedCacheIndex].moduleMap;
            
            if (currentModuleMap) {
                for (auto&& [key, module] : *currentModuleMap) {
                    if (m_state.filter(module)) {
                        renderCard(module, baseCardCache, selectedCard);
                    }
                }
            }
        } else {
            ImGui::Text("No caches available");
        }
        
        ImGui::End();
    }
    void HardwareView::renderCacheSelection() {
        ImGui::Text("Cache:");
        ImGui::SameLine();
        
        if (!m_caches.empty()) {
            std::vector<const char*> cacheNames;
            for (const auto& cache : m_caches) {
                cacheNames.push_back(cache.name.c_str());
            }
            
            if (ImGui::Combo("##Cache", &m_state.selectedCacheIndex, cacheNames.data(), cacheNames.size())) {
            }
        }
    }
    void HardwareView::renderFilterSelection() {
        ImGui::Text("Filter:");
        ImGui::SameLine();
        
        //TODO: proper filters
        const char* filterNames[] = { "All", "Cards Only", "Bases Only", "CPUs Only", "CPU Bases Only", "By Type", "By Name" };
        int currentFilter = static_cast<int>(m_state.filterType);
        
        if (ImGui::Combo("##Filter", &currentFilter, filterNames, IM_ARRAYSIZE(filterNames))) {
            m_state.filterType = static_cast<FilterType>(currentFilter);
            
            switch (m_state.filterType) {
                case FilterType::All:
                    m_state.filter = showAll();
                    m_state.filterName = "All";
                    break;
                case FilterType::CardsOnly:
                    m_state.filter = showCardsOnly();
                    m_state.filterName = "Cards Only";
                    break;
                case FilterType::BasesOnly:
                    m_state.filter = showBasesOnly();
                    m_state.filterName = "Bases Only";
                    break;
                case FilterType::CpusOnly:
                    m_state.filter = showCpusOnly();
                    m_state.filterName = "CPUs Only";
                    break;
                case FilterType::CpuBasesOnly:
                    m_state.filter = showCpuBasesOnly();
                    m_state.filterName = "CPU Bases Only";
                    break;
                case FilterType::ByType:
                    m_state.filterName = "By Type";
                    break;
                case FilterType::ByName:
                    m_state.filterName = "By Name";
                    break;
            }
        }
        
        //TODO: Proper types
        if (m_state.filterType == FilterType::ByType) {
            ImGui::SameLine();
            const char* cardTypeNames[] = { "ERROR", "GPU", "CPU", "Memory", "Storage" }; 
            int currentCardType = static_cast<int>(m_state.selectedCardType);
            if (ImGui::Combo("##CardType", &currentCardType, cardTypeNames, IM_ARRAYSIZE(cardTypeNames))) {
                m_state.selectedCardType = static_cast<HwTool::cardType>(currentCardType);
                m_state.filter = showByType(m_state.selectedCardType);
            }
        }
        
        if (m_state.filterType == FilterType::ByName) {
            ImGui::SameLine();
            char nameBuffer[256];
            strncpy(nameBuffer, m_state.namePattern.c_str(), sizeof(nameBuffer));
            if (ImGui::InputText("##NamePattern", nameBuffer, sizeof(nameBuffer))) {
                m_state.namePattern = nameBuffer;
                m_state.filter = showByName(m_state.namePattern);
            }
        }
    }
    
    HardwareView::ModuleFilter HardwareView::showAll() {
        return [](const HwTool::Module& m) { return true; };
    }
    
    HardwareView::ModuleFilter HardwareView::showCardsOnly() {
        return [](const HwTool::Module& m) { return HwTool::Utils::isCard(m); };
    }
    
    HardwareView::ModuleFilter HardwareView::showBasesOnly() {
        return [](const HwTool::Module& m) { return HwTool::Utils::isBase(m); };
    }
    
    HardwareView::ModuleFilter HardwareView::showCpusOnly() {
        return [](const HwTool::Module& m) { return HwTool::Utils::isCpu(m); };
    }
    
    HardwareView::ModuleFilter HardwareView::showCpuBasesOnly() {
        return [](const HwTool::Module& m) { return HwTool::Utils::isCpuBase(m); };
    }
    
    HardwareView::ModuleFilter HardwareView::showByType(HwTool::cardType type) {
        return [type](const HwTool::Module& m) { return m.type == type; };
    }
    
    HardwareView::ModuleFilter HardwareView::showByName(const std::string& namePattern) {
        return [namePattern](const HwTool::Module& m) { 
            return m.name.find(namePattern) != std::string::npos; 
        };
    }
}