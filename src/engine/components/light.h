#pragma once
#include "../component.h"
#include <glm/glm.hpp>

class Light : public Component {
public:
    enum class Type {
        Directional,
        Point,
        Spot
    };

    Light();
    virtual ~Light() = default;

    // Core functionality
    virtual void render(Shader& shader) override;
    virtual void onGUI() override;

    // Properties
    Type type;
    glm::vec3 color;
    float intensity;
    float range;
    float spotAngle;
    bool castShadows;

    // Getters
    glm::vec3 getDirection() const;
    glm::vec3 getPosition() const;

    // Serialization
    virtual void serialize(json& j) const override;
    virtual void deserialize(const json& j) override;

    virtual std::string getTypeName() const override {
        return "Light";
    }

    // Light properties access
    Type getLightType() const { return type; }
    void setLightType(Type newType) { type = newType; }

    const glm::vec3 &getColor() const { return color; }
    void setColor(const glm::vec3 &newColor) { color = newColor; }

    float getIntensity() const { return intensity; }
    void setIntensity(float value) { intensity = value; }

    float getRange() const { return range; }
    void setRange(float value) { range = value; }

    float getSpotAngle() const { return spotAngle; }
    void setSpotAngle(float angle) { spotAngle = angle; }

    bool getCastShadows() const { return castShadows; }
    void setCastShadows(bool value) { castShadows = value; }
};
