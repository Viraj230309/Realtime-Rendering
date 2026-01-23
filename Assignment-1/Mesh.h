#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaderClass.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct TextureInfo {
    unsigned int id;
    std::string type;   // baseColorMap / normalMap / metalRoughMap / emissiveMap
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureInfo> textures;

    unsigned int VAO, VBO, EBO;

    Mesh(std::vector<Vertex> verts,
        std::vector<unsigned int> inds,
        std::vector<TextureInfo> tex);

    void Draw(Shader& shader); // no const now

private:
    void setupMesh();
};

#endif
