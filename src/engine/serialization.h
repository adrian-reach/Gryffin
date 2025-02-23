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

// GLM Vector3 serialization
namespace glm {
    template<typename T>
    void to_json(json& j, const glm::vec<3, T>& v) {
        j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
    }

    template<typename T>
    void from_json(const json& j, glm::vec<3, T>& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
    }

    // Quaternion serialization
    template<typename T>
    void to_json(json& j, const glm::qua<T, glm::defaultp>& q) {
        j = json{{"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z}};
    }

    template<typename T>
    void from_json(const json& j, glm::qua<T, glm::defaultp>& q) {
        j.at("w").get_to(q.w);
        j.at("x").get_to(q.x);
        j.at("y").get_to(q.y);
        j.at("z").get_to(q.z);
    }
}
