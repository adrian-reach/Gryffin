/**
 * @file shader.h
 * @brief Shader class for handling shader programs
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    printf("Loading shaders from:\n  Vertex: %s\n  Fragment: %s\n", vertexPath, fragmentPath);

    try
    {
        // First check if files exist
        if (!std::ifstream(vertexPath).good())
        {
            printf("ERROR: Vertex shader file does not exist: %s\n", vertexPath);
            throw std::runtime_error("Vertex shader file not found");
        }
        if (!std::ifstream(fragmentPath).good())
        {
            printf("ERROR: Fragment shader file does not exist: %s\n", fragmentPath);
            throw std::runtime_error("Fragment shader file not found");
        }

        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Print shader code for debugging
        // printf("Vertex Shader Code:\n%s\n", vertexCode.c_str());
        // printf("Fragment Shader Code:\n%s\n", fragmentCode.c_str());
    }
    catch (std::ifstream::failure e)
    {
        printf("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n");
        printf("Vertex shader path: %s\n", vertexPath);
        printf("Fragment shader path: %s\n", fragmentPath);
        throw std::runtime_error("Failed to read shader files");
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Create and link shader program
    ID = glCreateProgram();
    if (ID == 0)
    {
        printf("Failed to create shader program\n");
        throw std::runtime_error("Failed to create shader program");
    }

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Query and validate uniform locations before deleting shaders
    GLint numUniforms;
    glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);
    printf("Active uniforms in shader program %d:\n", ID);

    // Pre-cache all uniform locations
    const char *expectedUniforms[] = {
        "model", "view", "projection", "viewPos",
        "lightPos", "lightColor", "objectColor"};

    for (const char *uniformName : expectedUniforms)
    {
        GLint location = glGetUniformLocation(ID, uniformName);
        if (location == -1)
        {
            printf("Warning: Expected uniform '%s' not found in shader program\n", uniformName);
        }
        uniformLocations[uniformName] = location;
    }

    // Print all active uniforms for debugging
    // for (GLint i = 0; i < numUniforms; i++)
    // {
    //     GLchar name[128];
    //     GLint size;
    //     GLenum type;
    //     glGetActiveUniform(ID, i, sizeof(name), NULL, &size, &type, name);
    //     GLint location = glGetUniformLocation(ID, name);
    //     printf("  %s (location: %d)\n", name, location);
    // }

    // Clean up shader objects
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Verify program is valid
    glValidateProgram(ID);
    GLint validProgram;
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &validProgram);
    if (!validProgram)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(ID, 1024, NULL, infoLog);
        printf("Warning: Initial shader program validation failed: %s\n", infoLog);
        // Don't throw here as the program might still work when proper state is set
    }
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::use()
{
    glUseProgram(ID);
}

GLint Shader::getUniformLocation(const std::string &name) const
{
    // Check if we've already cached this uniform location
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end())
    {
        return it->second;
    }

    // Get the uniform location and cache it
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        printf("Warning: Uniform '%s' not found in shader program %d\n", name.c_str(), ID);
    }
    uniformLocations[name] = location;
    return location;
}

void Shader::setBool(const std::string &name, bool value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1)
    {
        glUniform1i(location, (int)value);
    }
}

void Shader::setInt(const std::string &name, int value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1)
    {
        glUniform1i(location, value);
    }
}

void Shader::setFloat(const std::string &name, float value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1)
    {
        glUniform1f(location, value);
    }
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    GLint location = getUniformLocation(name);
    if (location != -1)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    GLint location = getUniformLocation(name);
    if (location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type.c_str(), infoLog);
            throw std::runtime_error("Shader compilation failed");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type.c_str(), infoLog);
            throw std::runtime_error("Shader program linking failed");
        }
    }
}
