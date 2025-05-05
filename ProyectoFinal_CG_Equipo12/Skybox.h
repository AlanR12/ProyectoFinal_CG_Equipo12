#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#ifdef _WIN32
    #include "SOIL2/SOIL2.h"
#else
    #include <soil2/SOIL2.h>
#endif

#include "Shader.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubemapTexture;

    GLuint loadCubemap(const std::vector<std::string>& faces);
    void setupSkybox(); // 🔹 Esta línea es la que faltaba
};

#endif
