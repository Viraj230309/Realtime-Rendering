#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // Program ID
    GLuint ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexFile, const char* fragmentFile);

    // Activate the shader
    void Activate();

    // Delete the shader program
    void Delete();

    // Uniform helper functions
    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

};



#endif
