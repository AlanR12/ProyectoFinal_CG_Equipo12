#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class Shader {
public:
    GLuint ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setMat4(const std::string& name, const float* value);
};

#endif