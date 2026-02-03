#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Shader;

class Texture
{
public:
    GLuint ID;
    GLenum type;

    Texture(const char* imageFile,
        GLenum texType,
        GLenum slot,
        GLenum internalFormat,
        GLenum pixelType);

    void texUnit(Shader& shader, const char* uniform, GLuint unit);
    void Bind()   const;
    void Unbind() const;
    void Delete();
};

#endif
