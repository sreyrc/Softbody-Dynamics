/*
**********************************************************
CS-560 Framework by Sreyash(Srey) Raychaudhuri
Project 4 - Physics Based Modeling
**********************************************************
* */

#include <glad/glad.h>

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/implot.h"

#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "Editor.h"

#include <glm/gtx/matrix_decompose.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Camera* camera);

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main() {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CS-560", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Create a camera
    Camera* camera = new Camera(glm::vec3(0.0f, 1.5f, 3.0f));

    // Soft body
    SoftBody* softBody = new SoftBody(4, 4, 4);

    // Create renderer
    Renderer* renderer = new Renderer(camera, softBody,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create an editor (for changing params with Dear Imgui)
    Editor* editor = new Editor(window);


    lastFrame = glfwGetTime();

    // Render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process all keyboard input
        processInput(window, camera);
       
        // Draw call
       renderer->Draw();

       // Physics Update. Multiple updates per frame for stability
       for (int i = 0; i < 10.0f; i++) {
           softBody->Update(deltaTime / 10.0f);
       }

        // If changes made with Imgui interface, update
        editor->Update(softBody, renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImPlot::DestroyContext();

    // Delete the rest of the stuff
    delete camera;
    delete renderer;
    delete editor;

    glfwTerminate();
    return 0;
}

// To make sure the viewport matches the new window dimensions; 
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// For (keyboard) input processing
void processInput(GLFWwindow* window, Camera* camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // For camera translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(MOVE_FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(MOVE_BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(MOVE_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(MOVE_RIGHT, deltaTime);

    // For Camera Rotation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera->ProcessKeyboard(TURN_UPWARDS, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera->ProcessKeyboard(TURN_DOWNWARDS, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera->ProcessKeyboard(TURN_LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera->ProcessKeyboard(TURN_RIGHT, deltaTime);

    // For Zooming
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera->ProcessKeyboard(ZOOM_IN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        camera->ProcessKeyboard(ZOOM_OUT, deltaTime);

    // To Reset to initial Camera positions
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera->ProcessKeyboard(RESET, deltaTime);
}
