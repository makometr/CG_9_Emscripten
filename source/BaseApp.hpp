#pragma once

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

class BaseApp {
public:
	BaseApp();
	BaseApp(const BaseApp &app) = delete;
    BaseApp& operator=(BaseApp&) = delete;
	void Run();
	~BaseApp();

protected:
	void main_loop();
	virtual void Start() = 0;
	virtual void Update(float dTime) = 0;
	virtual void End() = 0;

private:
	GLFWwindow *window = nullptr;
	float currentTime, lastTime, dTime;
};