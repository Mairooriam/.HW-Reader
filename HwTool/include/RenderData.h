#pragma once
#include <unordered_map>
#include <vector>

#include "types.h"

namespace HwTool {
    struct RenderData {
        const ModuleMap* modules;
        const std::unordered_map<std::string, std::string>* cacheBaseLink;
        const std::unordered_map<std::string, std::string>* cacheBaseLinkReverse;
        const std::unordered_map<std::string, Module>* cacheModules;
        const std::unordered_map<std::string, Module>* cacheBase;
        const std::unordered_map<std::string, Module>* cacheCard;
        std::vector<std::string> availableCards;
        std::vector<std::string> chain;
        std::vector<std::string> cardChain;
        
        RenderData() = default;
        
        RenderData(const ModuleMap& mod,
                   const std::unordered_map<std::string, std::string>& baseLink,
                   const std::unordered_map<std::string, std::string>& BaseLinkReverse,
                   const std::unordered_map<std::string, Module>& modules,
                   const std::unordered_map<std::string, Module>& base,
                   const std::unordered_map<std::string, Module>& card,
                   std::vector<std::string> cards,
                   std::vector<std::string> chain,
                   std::vector<std::string> cardChain)
            : modules(&mod),
              cacheBaseLink(&baseLink),
              cacheBaseLinkReverse(&BaseLinkReverse),
              cacheModules(&modules),
              cacheBase(&base),
              cacheCard(&card),
              availableCards(std::move(cards)),
              chain(std::move(chain)),
              cardChain(std::move(cardChain)) {}
    };
}  // namespace HwTool