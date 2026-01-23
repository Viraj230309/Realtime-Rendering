#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"

// -------------------- Window --------------------
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

// -------------------- Camera --------------------
Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

// -------------------- State --------------------
int currentShader = 0;

// -------------------- Callbacks --------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -------------------- Input --------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.Inputs(window);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentShader = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currentShader = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currentShader = 2;
}

// -------------------- Main --------------------
int main()
{
    // GLFW init
    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Reflectance Models Demo", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD init (correct way)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialise GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // -------------------- Shaders --------------------
    Shader phongShader("default.vert", "phong.frag");
    Shader cookShader("default.vert", "cook_torrance.frag");
    Shader toonShader("default.vert", "toon.frag");

    Shader* shaders[] = { &phongShader, &cookShader, &toonShader };

    // -------------------- Model --------------------
    Model model("Models/japanese_teapot.glb");

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // -------------------- Lighting --------------------
    glm::vec3 lightPos = glm::vec3(-2.0f, 2.0f, 2.0f);
    glm::vec3 lightColor = glm::vec3(1.0f);

    // -------------------- Render Loop --------------------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader& shader = *shaders[currentShader];
        shader.Activate();

        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        camera.Matrix(shader, "camMatrix");

        shader.setMat4("model", modelMatrix);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("camPos", camera.Position);

        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
