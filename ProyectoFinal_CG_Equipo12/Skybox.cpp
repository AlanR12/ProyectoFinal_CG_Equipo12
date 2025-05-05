#include "Skybox.h"

#ifdef _WIN32
    #include "SOIL2/SOIL2.h"
#else
    #include <soil2/SOIL2.h>
#endif

#include <iostream>

GLfloat skyboxVertices[] = {
    // posiciones        
    -10.0f,  10.0f, -10.0f,  -10.0f, -10.0f, -10.0f,   10.0f, -10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,   10.0f,  10.0f, -10.0f,  -10.0f,  10.0f, -10.0f,

    -10.0f, -10.0f,  10.0f,  -10.0f, -10.0f, -10.0f,  -10.0f,  10.0f, -10.0f,
    -10.0f,  10.0f, -10.0f,  -10.0f,  10.0f,  10.0f,  -10.0f, -10.0f,  10.0f,

     10.0f, -10.0f, -10.0f,   10.0f, -10.0f,  10.0f,   10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,   10.0f,  10.0f, -10.0f,   10.0f, -10.0f, -10.0f,

    -10.0f, -10.0f,  10.0f,  -10.0f,  10.0f,  10.0f,   10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,   10.0f, -10.0f,  10.0f,  -10.0f, -10.0f,  10.0f,

    -10.0f,  10.0f, -10.0f,   10.0f,  10.0f, -10.0f,   10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,  -10.0f,  10.0f,  10.0f,  -10.0f,  10.0f, -10.0f,

    -10.0f, -10.0f, -10.0f,  -10.0f, -10.0f,  10.0f,   10.0f, -10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,  -10.0f, -10.0f,  10.0f,   10.0f, -10.0f,  10.0f
};

Skybox::Skybox(const std::vector<std::string>& faces) {
    cubemapTexture = loadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cerr << "Error: la textura del cubemap no se cargó correctamente.\n";
    }
    setupSkybox();
}

void Skybox::setupSkybox() {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (GLuint i = 0; i < faces.size(); i++) {
        int width, height, nrChannels;
        unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGB);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            SOIL_free_image_data(data);
        } else {
            std::cout << "Cubemap texture failed to load at: " << faces[i] << std::endl;
            SOIL_free_image_data(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);
    shader.use();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    shader.setMat4("view", &viewNoTranslation[0][0]);
    shader.setMat4("projection", &projection[0][0]);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
