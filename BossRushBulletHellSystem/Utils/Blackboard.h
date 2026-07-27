#pragma once
#include <unordered_map>
#include <any>

using BlackboardId = unsigned int;

class Blackboard final
{
public:
    template<typename T>
    void Set(BlackboardId id, T&& value)
    {
        m_BlackboardMap[id] = std::forward<T>(value);
    }

    template<typename T>
    void Get(BlackboardId id)
    {
        return std::any_cast<T&&>(m_BlackboardMap[id]);
    }
private:
	std::unordered_map<BlackboardId, std::any> m_BlackboardMap;
};