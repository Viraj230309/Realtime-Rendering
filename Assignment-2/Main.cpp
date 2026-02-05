#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "shaderClass.h"
#include "Camera.h"
#include "Model.h"
#include "stb_image.h"

// -------------------- Window --------------------
constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;

// -------------------- Callbacks -----------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -------------------- HDR Loader ----------------
unsigned int loadHDR(const char* path)
{
    stbi_set_flip_vertically_on_load(true);

    int w, h, n;
    float* data = stbi_loadf(path, &w, &h, &n, 0);

    if (!data)
    {
        std::cout << "FAILED TO LOAD HDR: " << path << std::endl;
        return 0;
    }

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB16F,
        w, h, 0, GL_RGB, GL_FLOAT, data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

float skyboxVertices[] = {
    -1,  1, -1,  -1, -1, -1,   1, -1, -1,
     1, -1, -1,   1,  1, -1,  -1,  1, -1,

    -1, -1,  1,  -1, -1, -1,  -1,  1, -1,
    -1,  1, -1,  -1,  1,  1,  -1, -1,  1,

     1, -1, -1,   1, -1,  1,   1,  1,  1,
     1,  1,  1,   1,  1, -1,   1, -1, -1,

    -1, -1,  1,  -1,  1,  1,   1,  1,  1,
     1,  1,  1,   1, -1,  1,  -1, -1,  1,

    -1,  1, -1,   1,  1, -1,   1,  1,  1,
     1,  1,  1,  -1,  1,  1,  -1,  1, -1,

    -1, -1, -1,  -1, -1,  1,   1, -1, -1,
     1, -1, -1,  -1, -1,  1,   1, -1,  1
};


// -------------------- Main ----------------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT,
        "Assignment 2 – Transmittance",
        nullptr, nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL())
    {
        std::cout << "Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // ---------- SKYBOX SETUP (CORRECT PLACE) ----------
    unsigned int skyVAO, skyVBO;

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(skyboxVertices),
        skyboxVertices,
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glBindVertexArray(0);

    // Camera MUST use this constructor
    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 6.0f));

    Shader skyShader("skybox.vert", "skybox.frag");
    skyShader.Activate();
    skyShader.setInt("hdrMap", 0);

    Shader glassShader("vertex.glsl", "fragment.glsl");
    Model glassModel1("Models/TeapotToBe.obj");   // OBJ, no textures
    Model glassModel2("Models/Bottle.obj");   // OBJ, no textures
    Model glassModel3("Models/Sphere.obj");   // OBJ, no textures

    unsigned int hdrTex = loadHDR("Models/Outside.hdr");

    glassShader.Activate();
    glassShader.setInt("hdrMap", 0);

    // --------------- RENDER LOOP ---------------
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. UPDATE CAMERA FIRST
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // 2. DRAW SKYBOX
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        skyShader.Activate();

        // camera.cameraMatrix = projection * view
        glm::mat4 vp = camera.cameraMatrix;

        // remove translation safely
        vp[3] = glm::vec4(0, 0, 0, 1);

        skyShader.setMat4("vp", vp);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex);

        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        // 3. DRAWING OBJECTS
        glassShader.Activate();
        camera.Matrix(glassShader, "camMatrix");
        glassShader.setVec3("cameraPos", camera.Position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex);

        // -------- TEAPOT --------
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(-5.0f, 0.0f, 0.0f));
        model1 = glm::rotate(model1, (float)glfwGetTime() * 0.6f, glm::vec3(0, 1, 0));
		model1 = glm::scale(model1, glm::vec3(0.9f));
        glassShader.setMat4("model", model1);
        glassModel1.Draw(glassShader);

        // -------- BOTTLE --------
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
        model2 = glm::rotate(model2, (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
        model2 = glm::scale(model2, glm::vec3(0.15f));
        glassShader.setMat4("model", model2);
        glassModel2.Draw(glassShader);

        // -------- SPHERE --------
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(5.0f, 0.0f, 0.0f));
        model3 = glm::rotate(model3, (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
		model3 = glm::scale(model3, glm::vec3(1.5f));
        glassShader.setMat4("model", model3);
        glassModel3.Draw(glassShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
