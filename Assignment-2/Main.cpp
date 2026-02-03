#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"

// ------- Window -------
constexpr unsigned int SCR_WIDTH = 1980;
constexpr unsigned int SCR_HEIGHT = 1080;

// ------- Camera -------
Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.5f, 5.0f));

// ------- Callback -------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Reflectance Models Demo", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // Shader
    Shader phongShader("default.vert", "phong.frag");
    Shader cookShader("default.vert", "cook_torrance.frag");
    Shader toonShader("default.vert", "toon.frag");

    // Model
    Model model("Models/TeapotToBe.glb");

    float ambient = 0.4f;
    float specularStr = 0.5f;
    float shininess = 32.0f;

    glm::mat4 baseModel =
        glm::scale(glm::mat4(0.5f), glm::vec3(0.5f));

    glm::vec3 positions[3] = {
        {-7.0f, -2.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 7.0f, -2.0f, 0.0f}
    };

    glm::vec3 lightPos(2.0f, 2.0f, 2.0f);
    glm::vec3 lightColor(1.0f);
    float lightAmbient = 0.2f;
    float lightDiffuse = 1.0f;
    float lightSpecular = 1.0f;

    Shader* potShaders[3] = {
    &phongShader,
    &cookShader,
    &toonShader
    };

    // Render loop 
    while (!glfwWindowShouldClose(window))
    {
        camera.Inputs(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Shader
        phongShader.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        camera.Matrix(phongShader, "camMatrix");

        phongShader.setVec3("lightPos", lightPos);
        phongShader.setVec3("lightColor", lightColor);
        phongShader.setVec3("camPos", camera.Position);

        float time = (float)glfwGetTime();

        for (int i = 0; i < 3; i++)
        {
            Shader& shader = *potShaders[i];
            shader.Activate();

            camera.updateMatrix(45.0f, 0.1f, 100.0f);
            camera.Matrix(shader, "camMatrix");

            shader.setVec3("lightPos", lightPos);
            shader.setVec3("lightColor", lightColor);
            shader.setFloat("lightAmbient", lightAmbient);
            shader.setFloat("lightDiffuse", lightDiffuse);
            shader.setFloat("lightSpecular", lightSpecular);

            shader.setVec3("camPos", camera.Position);

            glm::mat4 modelMat = baseModel;
            modelMat = glm::translate(modelMat, positions[i]);
            modelMat = glm::rotate(
                modelMat,
                time * (0.6f + i * 0.1f),
                glm::vec3(0.2f, 1, 0.3f)
            );

            shader.setMat4("model", modelMat);
            if (&shader == &phongShader)
            {
                shader.setFloat("ambientStrength", ambient);
                shader.setFloat("specularStrength", specularStr);
                shader.setFloat("shininess", shininess);
            }
            else if (&shader == &cookShader)
            {
                shader.setFloat("lightAmbient", lightAmbient);
                shader.setFloat("lightDiffuse", lightDiffuse);
            }
            model.Draw(shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
