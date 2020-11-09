#pragma once

#include <GLFW/glfw3.h>

#include "Camera.hpp"

class CameraMoveCallbackManager {
public:
    CameraMoveCallbackManager() = default;
    void setCallbacks(GLFWwindow* winPtr, Camera* camPtr) const;
    void applyPlayerMoveControllerChanges(GLfloat deltaTime) const;
    bool getCameraActiveStatus() const;
};