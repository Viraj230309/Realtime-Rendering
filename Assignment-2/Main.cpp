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
    Model glassModel("Models/TeapotToBe.obj");   // OBJ, no textures
    //Model glassModel("Models/Bottle.obj");   // OBJ, no textures
    //Model glassModel("Models/Donut.obj");   // OBJ, no textures

    unsigned int hdrTex = loadHDR("Models/Studio.hdr");

    glassShader.Activate();
    glassShader.setInt("hdrMap", 0);

	// --------------- RENDER LOOP ---------------
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. UPDATE CAMERA FIRST
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT,
            0.1f, 100.0f
        );

        // 2. DRAW SKYBOX
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        skyShader.Activate();

        glm::mat4 view = camera.cameraMatrix;
        view[3] = glm::vec4(0, 0, 0, 1); // remove translation

        skyShader.setMat4("view", view);
        skyShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex);

        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        // 3. DRAW GLASS OBJECT
        glassShader.Activate();
        camera.Matrix(glassShader, "camMatrix"); // FULL view + projection
        glassShader.setVec3("cameraPos", camera.Position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model,
            (float)glfwGetTime() * 0.6f,
            glm::vec3(0, 1, 0));

        glassShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex);

        glassModel.Draw(glassShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
