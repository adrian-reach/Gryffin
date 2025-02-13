/**
 * @file: shader.h
 * @brief: Shader class for handling shader programs
 */

#pragma once
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

public:
    // Get the shader program ID
    unsigned int getProgramID() const { return ID; }
    GLint getUniformLocation(const std::string &name) const;

private:
    unsigned int ID;
    mutable std::unordered_map<std::string, GLint> uniformLocations;
    void checkCompileErrors(unsigned int shader, std::string type);
};
