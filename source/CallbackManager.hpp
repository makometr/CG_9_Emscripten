#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <algorithm>
#include <iostream>

#include "Camera.hpp"

static Camera* camera = nullptr;
static std::array<bool, 1024> keys;

// Function prototypes
inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
inline void mouse_callback(GLFWwindow* window, double xpos, double ypos);
inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


class CallbackManager {
public:
    CallbackManager() = default;
    void setCallbacks(GLFWwindow* winPtr, Camera* camPtr);
};


void CallbackManager::setCallbacks(GLFWwindow* winPtr, Camera* camPtr) {
    camera = camPtr;
    glfwSetKeyCallback(winPtr, key_callback);
    glfwSetCursorPosCallback(winPtr, mouse_callback);
    glfwSetScrollCallback(winPtr, scroll_callback);
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