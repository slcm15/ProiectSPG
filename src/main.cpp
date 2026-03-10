#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "Shader.h"

// Prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void generateDuneMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                      int rows, int cols, float width, float depth);

// Screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // GLFW initialization and configuration
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sand Dune with Sky", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Load textures
    unsigned int sandTex = loadTexture("C:/Users/Larisa/Downloads/CLion_OpenGL-master/CLion_OpenGL-master/deps/textures/nisip3.jpg");
    unsigned int skyTex  = loadTexture("C:/Users/Larisa/Downloads/CLion_OpenGL-master/CLion_OpenGL-master/deps/textures/cer.jpg");

    // Build and activate shader
    Shader shader(
        "C:/Users/Larisa/Downloads/CLion_OpenGL-master/CLion_OpenGL-master/shaders/shader.vs",
        "C:/Users/Larisa/Downloads/CLion_OpenGL-master/CLion_OpenGL-master/shaders/shader.fs"
    );
    shader.use();
    shader.setInt("texture1", 0);

    // =========================
    // SKY GEOMETRY (cube room)
    // =========================
    float skyVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.5f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.5f,

        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.5f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.5f,

        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.5f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.5f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.5f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.5f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // =========================
    // DUNE GEOMETRY
    // =========================
    std::vector<float> duneVertices;
    std::vector<unsigned int> duneIndices;
    generateDuneMesh(duneVertices, duneIndices, 120, 120, 20.0f, 20.0f);

    unsigned int duneVAO, duneVBO, duneEBO;
    glGenVertexArrays(1, &duneVAO);
    glGenBuffers(1, &duneVBO);
    glGenBuffers(1, &duneEBO);

    glBindVertexArray(duneVAO);

    glBindBuffer(GL_ARRAY_BUFFER, duneVBO);
    glBufferData(GL_ARRAY_BUFFER, duneVertices.size() * sizeof(float), duneVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, duneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, duneIndices.size() * sizeof(unsigned int), duneIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f, 200.0f);
        shader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 6.0f, 18.0f),
            glm::vec3(0.0f, 1.5f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        shader.setMat4("view", view);

        // =========================
        // Draw sky
        // =========================
        glBindVertexArray(skyVAO);
        glBindTexture(GL_TEXTURE_2D, skyTex);

        glm::mat4 skyModel = glm::mat4(1.0f);
        skyModel = glm::scale(skyModel, glm::vec3(80.0f));
        shader.setMat4("model", skyModel);

        glDrawArrays(GL_TRIANGLES, 0, 30);

        // =========================
        // Draw dune
        // =========================
        glBindVertexArray(duneVAO);
        glBindTexture(GL_TEXTURE_2D, sandTex);

        glm::mat4 duneModel = glm::mat4(1.0f);
        shader.setMat4("model", duneModel);

        glDrawElements(GL_TRIANGLES, (GLsizei)duneIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);

    glDeleteVertexArrays(1, &duneVAO);
    glDeleteBuffers(1, &duneVBO);
    glDeleteBuffers(1, &duneEBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void generateDuneMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                      int rows, int cols, float width, float depth) {
    vertices.clear();
    indices.clear();

    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;

    float duneHeight = 3.0f;
    float sigmaX = 4.5f;
    float sigmaZ = 2.0f;

    for (int i = 0; i <= rows; i++) {
        for (int j = 0; j <= cols; j++) {
            float x = -halfW + width * ((float)j / cols);
            float z = -halfD + depth * ((float)i / rows);

            float y = duneHeight * std::exp(-((x * x) / (2.0f * sigmaX * sigmaX)
                                            + (z * z) / (2.0f * sigmaZ * sigmaZ)));

            float u = ((float)j / cols) * 6.0f;
            float v = ((float)i / rows) * 6.0f;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int topLeft = i * (cols + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (cols + 1) + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}