#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

#include "Mesh.h"         
#include "Texture.h"
#include "shaderClass.h"

class Model {
public:
    std::vector<Mesh> meshes;
    std::vector<TextureInfo> loadedTextures;

    Model(const char* path);
    void Draw(Shader& shader);

private:
    std::string directory;
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};


#endif
