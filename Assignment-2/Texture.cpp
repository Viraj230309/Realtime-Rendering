#include "Texture.h"
#include "shaderClass.h"

#include <stb/stb_image.h>
#include <iostream>

Texture::Texture(const char* imageFile,
    GLenum texType,
    GLenum slot,
    GLenum internalFormat,
    GLenum pixelType)
{
    type = texType;

    int width, height, channels;
    unsigned char* data = stbi_load(imageFile, &width, &height, &channels, 0);

    if (!data)
    {
        std::cout << "❌ Failed to load texture: " << imageFile << "\n";
        return;
    }
    else
    {
        std::cout << "✔ Loaded texture: " << imageFile
            << " (" << width << "x" << height << ", " << channels << " channels)\n";
    }

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(type, ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(type, 0, internalFormat, width, height, 0, format, pixelType, data);
    glGenerateMipmap(type);

    stbi_image_free(data);
    glBindTexture(type, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, uniform), unit);
}

void Texture::Bind() const
{
    glBindTexture(type, ID);
}

void Texture::Unbind() const
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}
