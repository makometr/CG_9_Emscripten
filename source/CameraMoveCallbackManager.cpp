#include <array>
#include <iostream>

#include "Camera.hpp"
#include "CameraMoveCallbackManager.hpp"


static Camera* camera = nullptr;
static std::array<bool, 1024> keys = {false};

// Callbacks prototypes (glfw requirments)
inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
inline void mouse_callback(GLFWwindow* window, double xpos, double ypos);
inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void CameraMoveCallbackManager::setCallbacks(GLFWwindow* winPtr, Camera* camPtr) {
    camera = camPtr;
    glfwSetKeyCallback(winPtr, key_callback);
    glfwSetCursorPosCallback(winPtr, mouse_callback);
    glfwSetScrollCallback(winPtr, scroll_callback);
}

void CameraMoveCallbackManager::applyPlayerMoveControllerChanges(GLfloat deltaTime) {
    if (keys[GLFW_KEY_W])
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera->ProcessKeyboard(RIGHT, deltaTime);
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        std::cout << key << std::endl;
        // if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        //     glfwSetWindowShouldClose(window, GL_TRUE);
        if (key >= 0 && key < 1024) {
            if(action == GLFW_PRESS)
                keys[key] = true;
            else if(action == GLFW_RELEASE)
                keys[key] = false;	
        }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera->ProcessMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera->ProcessMouseScroll(yoffset);
}