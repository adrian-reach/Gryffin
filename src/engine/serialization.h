#pragma once
#include "iserializable.h"
#include <json/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using json = nlohmann::json;

// GLM type serialization declarations
namespace glm {
    void to_json(json& j, const vec3& v);
    void from_json(const json& j, vec3& v);
    void to_json(json& j, const quat& q);
    void from_json(const json& j, quat& q);
}
