#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <iostream>

#include "Camera.hpp"

class CallbackManager {
public:
    CallbackManager() = default;
    void setCallbacks(GLFWwindow* winPtr, Camera* camPtr);
};