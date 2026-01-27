#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Shader
    Shader phongShader("default.vert", "phong.frag");
    Shader cookShader("default.vert", "cook_torrance.frag");
    Shader toonShader("default.vert", "toon.frag");

    // Model
    Model model("Models/Teapot.glb");

    glm::mat4 baseModel =
        glm::scale(glm::mat4(0.5f), glm::vec3(0.2f));

    glm::vec3 positions[3] = {
        {-30.0f, 0.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},
        { 30.0f, 0.0f, 0.0f}
    };

    glm::vec3 lightPos(2.0f, 2.0f, 2.0f);
    glm::vec3 lightColor(1.0f);

    Shader* potShaders[3] = {
    &phongShader,
    &cookShader,
    &toonShader
    };

    // ------- Render loop -------
    while (!glfwWindowShouldClose(window))
    {
        camera.Inputs(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Light Controls");
        ImGui::SliderFloat3("Light Position", &lightPos.x, -5.0f, 5.0f);
        ImGui::ColorEdit3("Light Color", &lightColor.x);
        ImGui::End();

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
            shader.setVec3("camPos", camera.Position);

            glm::mat4 modelMat = baseModel;
            modelMat = glm::translate(modelMat, positions[i]);
            modelMat = glm::rotate(
                modelMat,
                time * (0.6f + i * 0.3f),
                glm::vec3(0.2f, 1, 0.5f)
            );

            shader.setMat4("model", modelMat);

            // Only Phong uses these
            if (&shader == &phongShader)
            {
                shader.setFloat("shininess", 32.0f);
                shader.setFloat("ks", 0.6f);
            }

            model.Draw(shader);
        }


        // ImGui render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
