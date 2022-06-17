#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>

class Shader {
public:
    Shader(std::string vertexShaderSource, std::string fragmentShaderSource);
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    ~Shader();

    void bind();
    void unbind();

    unsigned int getUniformLocation(const char* name);
private:
    unsigned int shaderProgram;

    unsigned int createShaderProgram(std::string vertexShaderSource, std::string fragmentShaderSource);
    unsigned int compileShader(std::string source, unsigned int shaderType);

    std::string readFile(std::string filePath);
};

#endif