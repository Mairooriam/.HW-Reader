#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include "Types.h"

inline std::string nextSuffix(std::string suffix) {
        int i = suffix.size() - 1;
        while (i >= 0 && suffix[i] == 'z') {
            suffix[i] = 'a';
            --i;
        }
        if (i < 0) {
            suffix.insert(suffix.begin(), 'a');
        } else {
            ++suffix[i];
        }
    return suffix;
}

template<typename T, typename Key>
concept HasContains = requires(T t, Key k) {
    { t.contains(k) } -> std::convertible_to<bool>;
};

template<typename StringContainer>
requires HasContains<StringContainer, std::string>
std::string incrementStr(const std::string& str, const StringContainer& currentStrs) {
    std::string suffix = "a";
    while (true) {
        std::string candidate = str + suffix;
        if (!currentStrs.contains(candidate)) {
            return candidate;
        }
        suffix = nextSuffix(suffix);
    }
}
