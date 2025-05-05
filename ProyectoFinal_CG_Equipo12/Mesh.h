#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    void Draw(GLuint shaderID);

private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};

#endif