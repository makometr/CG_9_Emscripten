#include <array>
#include <iostream>

#include "Camera.hpp"
#include "CameraMoveCallbackManager.hpp"


static Camera* camera = nullptr;
static std::array<bool, 1024> keys = {false};
static bool isCameraActive = true;
// TODO Create class with State of App like S in MVS pattern.
// There is tendency this class can "eat" all state-app variables.
 
// Callbacks prototypes (glfw requirments)
inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
inline void mouse_callback(GLFWwindow* window, double xpos, double ypos);
inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void CameraMoveCallbackManager::setCallbacks(GLFWwindow* winPtr, Camera* camPtr) const {
    camera = camPtr;
    glfwSetKeyCallback(winPtr, key_callback);
    glfwSetCursorPosCallback(winPtr, mouse_callback);
    glfwSetScrollCallback(winPtr, scroll_callback);
}

void CameraMoveCallbackManager::applyPlayerMoveControllerChanges(GLfloat deltaTime) const {
    if (keys[GLFW_KEY_W])
        camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

bool CameraMoveCallbackManager::getCameraActiveStatus() const {
    return isCameraActive;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        std::cout << key << std::endl;
        // if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        //     glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            isCameraActive = !isCameraActive;
        if (key >= 0 && key < 1024) {
            if(action == GLFW_PRESS)
                keys[key] = true;
            else if(action == GLFW_RELEASE)
                keys[key] = false;	
        }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // TODO remove this is statament -> callback can change only state class var
    if (isCameraActive)
        camera->ProcessMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // TODO remove this is statament -> callback can change only state class var
    if (isCameraActive)
	    camera->ProcessMouseScroll(yoffset);
}