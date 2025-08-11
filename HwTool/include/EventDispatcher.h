// EventDispatcher.h
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <type_traits>

class EventDispatcher {
public:
    template<typename Event>
    using Callback = std::function<void(const Event&)>;

    template<typename Event>
    void subscribe(Callback<Event> cb) {
        auto idx = std::type_index(typeid(Event));
        m_callbacks[idx].push_back([cb = std::move(cb)](const void* ptr) {
            const Event* event = static_cast<const Event*>(ptr);
            cb(*event);
        });
    }

    template<typename Event>
    void notify(const Event& event) {
        auto idx = std::type_index(typeid(Event));
        auto it = m_callbacks.find(idx);
        if (it != m_callbacks.end()) {
            for (auto& cb : it->second) {
                cb(static_cast<const void*>(&event));
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_callbacks;

    template<typename Event>
    std::vector<Callback<Event>>& getCallbacks() {
        auto idx = std::type_index(typeid(Event));
        if (m_callbacks.find(idx) == m_callbacks.end()) {
            m_callbacks[idx] = {};
        }
        auto& baseVec = m_callbacks[idx];
        static std::vector<Callback<Event>> typedVec;
        typedVec.clear();
        for (auto& baseCb : baseVec) {
            typedVec.push_back([baseCb](const Event& e) {
                baseCb(static_cast<const void*>(&e));
            });
        }
        return typedVec;
    }
};