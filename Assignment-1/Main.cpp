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
    Model model("Models/Bottle.glb");

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

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 260), ImGuiCond_Once);

        // Light Controls
        ImGui::Begin("Light Controls", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        // Position 
        ImGui::Text("Light Position");
        ImGui::Separator();
        ImGui::SliderFloat3("Position", &lightPos.x, -10.0f, 10.0f);

        // Color 
        ImGui::Spacing();
        ImGui::Text("Light Color");
        ImGui::Separator();
        ImGui::ColorEdit3("Color", &lightColor.x);

        // Intensities 
        ImGui::Spacing();
        ImGui::Text("Light Intensity");
        ImGui::Separator();
        ImGui::SliderFloat("Ambient", &lightAmbient, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &lightDiffuse, 0.0f, 2.0f);
        ImGui::SliderFloat("Specular", &lightSpecular, 0.0f, 2.0f);
        ImGui::SetNextWindowPos(ImVec2(20, 300), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 170), ImGuiCond_Once);
        static float roughness = 0.6f;
        ImGui::SliderFloat("Roughness", &roughness, 0.05f, 1.0f);

        ImGui::Begin("Phong Material", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular Strength", &specularStr, 0.0f, 2.0f);
        ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f);

        ImGui::End();

        // Info
        ImGui::Spacing();
        ImGui::TextDisabled("Same light used for all shaders");

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
                shader.setFloat("roughness", roughness); // add a slider
                shader.setFloat("lightAmbient", lightAmbient);
                shader.setFloat("lightDiffuse", lightDiffuse);
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
