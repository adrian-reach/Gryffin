#pragma once
#include <json/json.hpp>

using json = nlohmann::json;

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual void serialize(json& j) const = 0;
    virtual void deserialize(const json& j) = 0;
};
