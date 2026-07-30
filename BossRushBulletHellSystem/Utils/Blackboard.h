#pragma once
#include <unordered_map>
#include <any>
#include <json.hpp>
#include <typeindex>
#include <algorithm>

using BlackboardId = unsigned int;

struct BlackboardType
{
    std::string Name;
    std::function<nlohmann::json(const std::any&)> Serialize;
    std::function<std::any(const nlohmann::json&)> Deserialize;
};

struct Blackboard final
{
    Blackboard();
    template<typename T>
    void Register(const std::string& name)
    {
        Types[std::type_index(typeid(T))] =
        {
            .Name = name,
            .Serialize = [](const std::any& value)
            {
                return nlohmann::json(
                    std::any_cast<const T&>(value)
                );
            },
            .Deserialize = [](const nlohmann::json& json)
            {
                return std::any(json.get<T>());
            }
        };
    }


    template<typename T>
    void Set(BlackboardId id, T value)
    {
        BlackboardMap[id] = std::forward<T>(value);
    }

    template<typename T>
    T& Get(BlackboardId id)
    {
        return std::any_cast<T&>(BlackboardMap.at(id));
    }
    std::unordered_map<std::type_index, BlackboardType> Types;
    std::unordered_map<BlackboardId, std::any> BlackboardMap;
};

inline void to_json(nlohmann::json& j, const Blackboard& board)
{
    for (auto& [id, value] : board.BlackboardMap)
    {
        auto& serializer = board.Types.at(value.type());

        j["BlackboardMap"][std::to_string(id)] =
        {
            { "Type", serializer.Name },
            { "Value", serializer.Serialize(value) }
        };
    }
}

inline void from_json(const nlohmann::json& j, Blackboard& board)
{
    for (auto& [id, entry] : j["BlackboardMap"].items())
    {
        std::string typeName = entry["Type"];

        auto it = std::find_if(board.Types.begin(), board.Types.end(), [typeName](const auto& pair)
            {
                return pair.second.Name == typeName;
            });


        board.BlackboardMap[std::stoul(id)] = it->second.Deserialize(entry["Value"]);
    }
}