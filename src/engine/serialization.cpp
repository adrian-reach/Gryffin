#include "serialization.h"

namespace glm {
    void to_json(json& j, const vec3& v) {
        j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
    }

    void from_json(const json& j, vec3& v) {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
    }

    void to_json(json& j, const quat& q) {
        j = json{{"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z}};
    }

    void from_json(const json& j, quat& q) {
        j.at("w").get_to(q.w);
        j.at("x").get_to(q.x);
        j.at("y").get_to(q.y);
        j.at("z").get_to(q.z);
    }
}
