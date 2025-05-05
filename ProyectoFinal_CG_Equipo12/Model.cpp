#include "Model.h"
#include <iostream>
#ifdef _WIN32
#include "SOIL2/SOIL2.h"
#else
#include <soil2/SOIL2.h>
#endif



Model::Model(const std::string &path) {
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    for (GLuint i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader.ID);
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (GLuint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (GLuint i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for (GLuint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        glm::vec3 vector{ 0.0f };

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec{ 0.0f };
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (GLuint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps;
        aiString str;

        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS) {
            GLuint id = TextureFromFile(str.C_Str(), directory);
            if (id != 0) {
                Texture texture = { id, "texture_diffuse", str.C_Str() };
                textures.push_back(texture);
            }
        }
        else {
            std::cerr << "Material no tiene textura difusa." << std::endl;
        }
    }
    std::cout << "Mesh con vértices cargados: " << vertices.size() << std::endl;
    return Mesh(vertices, indices, textures);
}

GLuint Model::TextureFromFile(const char* path, const std::string& directory) {
    std::string filename = directory + "/" + std::string(path);

    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    // >>>>>> Verificación crítica: image, width y height <<<<<<
    if (!image || width <= 0 || height <= 0) {
        std::cerr << "ERROR::TEXTURE::Failed to load texture: " << filename
            << " (width=" << width << ", height=" << height << ")" << std::endl;
        SOIL_free_image_data(image); // Libera memoria incluso si image es nullptr
        return 0;
    }

    // >>>>>> Configura la alineación de píxeles <<<<<<
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Evita problemas con imágenes de ancho no múltiplo de 4

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // >>>>>> Parámetros de textura obligatorios <<<<<<
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(image);
    return textureID;
}
