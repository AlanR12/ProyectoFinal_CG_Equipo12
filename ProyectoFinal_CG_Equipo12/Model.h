#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    Model(const std::string& path);
    void Draw(Shader& shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    GLuint TextureFromFile(const char* path, const std::string& directory);
};

#endif