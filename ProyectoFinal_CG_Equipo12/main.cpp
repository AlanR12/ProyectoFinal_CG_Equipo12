#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"

const GLuint WIDTH = 800, HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);
void DoMovement();

std::string getExecutablePath() {
    char buffer[4096];
#ifdef _WIN32
    DWORD length = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    if (length == 0) return "";
#else
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (length == -1) return "";
#endif
    return std::string(buffer, length);
}

std::string getBasePath() {
    std::string fullPath = getExecutablePath();
    size_t found = fullPath.find_last_of("/\\");
    return fullPath.substr(0, found);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Escena Multiplataforma", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    std::string basePath = getBasePath();

#ifdef _WIN32
    Shader shader("Shaders/model.vs", "Shaders/model.frag");
    Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.frag");
#else
    Shader shader((basePath + "/../shaders/model.vs").c_str(), (basePath + "/../shaders/model.frag").c_str());
    Shader skyboxShader((basePath + "/../shaders/skybox.vs").c_str(), (basePath + "/../shaders/skybox.frag").c_str());
#endif

    std::vector<Model> modelos;
#ifdef _WIN32
    modelos.push_back(Model("models/compu.obj"));
    modelos.push_back(Model("models/silla.obj"));
    modelos.push_back(Model("models/escritorio.obj"));
    modelos.push_back(Model("models/Salon.obj"));
#else
    modelos.push_back(Model((basePath + "/../models/compu.obj").c_str()));
    modelos.push_back(Model((basePath + "/../models/silla.obj").c_str()));
    modelos.push_back(Model((basePath + "/../models/escritorio.obj").c_str()));
    modelos.push_back(Model((basePath + "/../models/Salon.obj").c_str()));
#endif

    std::vector<glm::vec3> posiciones = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-3.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
    };

    std::vector<glm::vec3> escalas = {
        glm::vec3(1.0f),
        glm::vec3(1.0f),
        glm::vec3(1.0f),
        glm::vec3(1.0f),
    };

    std::vector<std::string> faces;
#ifdef _WIN32
    faces = {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
#else
    faces = {
        basePath + "/../textures/skybox/right.jpg",
        basePath + "/../textures/skybox/left.jpg",
        basePath + "/../textures/skybox/top.jpg",
        basePath + "/../textures/skybox/bottom.jpg",
        basePath + "/../textures/skybox/front.jpg",
        basePath + "/../textures/skybox/back.jpg"
    };
#endif

    Skybox skybox(faces);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Skybox
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::value_ptr(glm::mat4(glm::mat3(view))));
        skyboxShader.setMat4("projection", glm::value_ptr(projection));
        skybox.Draw(skyboxShader, view, projection);

        // Modelos
        shader.use();
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        for (size_t i = 0; i < modelos.size(); i++) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, posiciones[i]);
            modelMatrix = glm::scale(modelMatrix, escalas[i]);
            shader.setMat4("model", glm::value_ptr(modelMatrix));
            modelos[i].Draw(shader);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void DoMovement() {
    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (action == GLFW_PRESS) keys[key] = true;
    else if (action == GLFW_RELEASE) keys[key] = false;
}

void MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos; lastY = ypos; firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}
