#include "Model.h"
#include <iostream>
#include "stb/stb_image.h"

unsigned int TextureFromFile(const char* path)
{
    unsigned int id;
    glGenTextures(1, &id);

    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);

    if (!data) { std::cout << "FAILED TEX LOAD: " << path << "\n"; return 0; }

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return id;
}

Model::Model(const char* path) { loadModel(path); }

void Model::Draw(Shader& shader)
{
    for (auto& mesh : meshes)
        mesh.Draw(shader);
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene) { std::cout << "ASSIMP ERR " << importer.GetErrorString(); return; }

    directory = path.substr(0, path.find_last_of("/\\"));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; i++)
        meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));

    for (unsigned i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    std::vector<TextureInfo> textures; // keep empty on purpose

    for (unsigned i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v;
        v.Position = { mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z };
        v.Normal = glm::normalize(glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        ));

        v.TexCoords = mesh->mTextureCoords[0] ?
            glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
            : glm::vec2(0);

        vertices.push_back(v);
    }

    for (unsigned i = 0; i < mesh->mNumFaces; i++)
        for (unsigned j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);

    //if (mesh->mMaterialIndex >= 0)
    //{
    //    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    //    auto load = [&](aiTextureType type, std::string name)
    //        {
    //            for (unsigned i = 0; i < mat->GetTextureCount(type); i++) {
    //                aiString file; mat->GetTexture(type, i, &file);
    //                std::string full = directory + "/" + file.C_Str();
    //                TextureInfo tex;
    //                tex.id = TextureFromFile(full.c_str());
    //                tex.type = name;
    //                tex.path = full;
    //                textures.push_back(tex);
    //                
    //            }
    //        };

    //    load(aiTextureType_DIFFUSE, "baseColorMap");
    //    load(aiTextureType_NORMALS, "normalMap");
    //    load(aiTextureType_METALNESS, "metalRoughMap");
    //    load(aiTextureType_EMISSIVE, "emissiveMap");
    //}

    return Mesh(vertices, indices, textures);
}
