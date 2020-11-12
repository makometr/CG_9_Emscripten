// Based on https://gist.github.com/ousttrue/0f3a11d5d28e365b129fe08f18f4e141

#include "BaseApp.hpp"
#include "Shader.hpp"


#include "imgui.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>

#include <array>
#include <algorithm>
#include <iostream>

#include "Camera.hpp"
#include "CameraMoveCallbackManager.hpp"
#include "ogl_objects/AxesOpenGL.hpp"
#include "ogl_objects/StandardCube.hpp"


// Window dimensions
// TODO move to state 
constexpr GLuint WIDTH = 800, HEIGHT = 600;
static bool isAxesEnabled = true;

struct Vertex
{
	glm::vec3 Pos;
	glm::vec3 Color;
	// glm::vec2 Tex;
};

enum class ViewType {
	Perspective,
	Orto
};

class App : public BaseApp {
	// Shader basicShader{"resources/shaders/basic.vs", "resources/shaders/basic.fs"};
	Shader axesShader{"resources/shaders/axes.vs", "resources/shaders/axes.fs"};
	Shader pointLightShader{"resources/shaders/point_light.vs", "resources/shaders/point_light.fs"};
	Shader lightedObjectShader{"resources/shaders/lighted.vs", "resources/shaders/lighted.fs"};

	Axes axes {};
	StandardCube lightCube {};
	StandardCube figure_cube {};

	CameraMoveCallbackManager cmcbManager {};
	Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};

	// For camera
	GLfloat currentFrame;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// Cube state
	float cubeRotateSpeed = 1;
	glm::vec3 cubePosition {0.0f};
	glm::vec3 cubeRotate {0.0f};
	GLfloat cubeRotateValue {0.0f};
	// glm::vec3 cubeScale {0.0f};
	GLfloat cubeScale {1.0f};

	// Light state
	glm::vec3 lightPosition {0.0f, 40.0f, 0.0f};
	glm::vec3 lightColor {1.0f};
	float specular {128.0f};

	void Start() override {
		glEnable(GL_DEPTH_TEST);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cmcbManager.setCallbacks(window, &camera);

		ImGui::StyleColorsLight();
		glClearColor(1.0, 0.87, 0.83, 1.0);

		axes.initBuffers();
		lightCube.initBuffers();
		figure_cube.initBuffers();
	}

	void Update(float dTime) override {
        // Set frame time
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 
		if (cmcbManager.getCameraActiveStatus())
			cmcbManager.applyPlayerMoveControllerChanges(deltaTime);

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui::SetNextWindowSize({300, 80}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({20, 20}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Camera");
			static int ProjectionType = 0;
			ImGui::RadioButton("Perspective", &ProjectionType, 0); ImGui::SameLine();
			ImGui::RadioButton("Orthographic", &ProjectionType, 1);
			ImGui::Checkbox("Axes enabled", &isAxesEnabled);
		ImGui::End();


		ImGui::SetNextWindowSize({300, 150}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({20, 110}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Light");
			ImGui::SliderFloat("Translate: X", &lightPosition.x, -100.0, 100.0);
			ImGui::SliderFloat("Translate: Y", &lightPosition.y, -100.0, 100.0);
			ImGui::SliderFloat("Translate: Z", &lightPosition.z, -100.0, 100.0);
			ImGui::SliderFloat3("Light Color", glm::value_ptr(lightColor), 0, 1);
			ImGui::SliderFloat("Specular###IDFORSLIDER", &specular, 0.001, 256.0);
		ImGui::End();

		ImGui::SetNextWindowSize({300, 220}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({20, 280}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Cube");
			ImGui::SliderFloat("Speed", &cubeRotateSpeed, -100.0, 100.0);
			ImGui::SliderFloat("Translate: X", &cubePosition.x, -100.0, 100.0);
			ImGui::SliderFloat("Translate: Y", &cubePosition.y, -100.0, 100.0);
			ImGui::SliderFloat("Translate: Z", &cubePosition.z, -100.0, 100.0);
			ImGui::SliderFloat("X Rotate", &cubeRotate.x, 0.0, 360.0);
			ImGui::SliderFloat("Y Rotate", &cubeRotate.y, 0.0, 360.0);
			ImGui::SliderFloat("Z Rotate", &cubeRotate.z, 0.0, 360.0);
			ImGui::SliderFloat("Scale", &cubeScale, -5.0, 5.0);
		ImGui::End();


        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection {1.0f};
		if (!ProjectionType)
        	projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		else
			projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f );


		// static const std::array<glm::vec3, 3> positions = {
		// 	glm::vec3(0.0f, 3.0f, 0.0f),
		// 	glm::vec3(0.0f, 2.0f, 0.0f),
		// 	glm::vec3(0.0f, 1.0f, 0.0f),
		// };
		// static const std::array<glm::vec3, 3> colors = {
		// 	glm::vec3{101.0f, 210.0f, 69.0f}/256.0f,
		// 	glm::vec3{31.0f, 171.0f, 205.0f}/256.0f,
		// 	glm::vec3{254.0f, 200.0f, 47.0f}/256.0f
		// };
		if (isAxesEnabled) {
			axes.draw(axesShader, [&projection, &view] (const Shader& shaderProg) {
				glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100));
				glm::mat4 transformMatrix = projection * view * model;
				GLuint transformLoc = glGetUniformLocation(shaderProg.getId(), "transform");
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));
			});
		}

		cubeRotateValue += (std::sin(glfwGetTime())+1) * cubeRotateSpeed/50;
		if (cubeRotateValue > 360.0f)
			cubeRotateValue -= 360.0f;
		cubeRotate = glm::vec3(cubeRotateValue);
		figure_cube.draw(lightedObjectShader, [&projection, &view, rotate=cubeRotate, cameraPos=camera.Position, pos=cubePosition, scale=cubeScale, lightPos=lightPosition, lightColor=lightColor, specular=specular] (const Shader& shaderProg) {
			glm::mat4 model {1.0f}; 
			model = glm::translate(model, glm::vec3(pos.x/10, pos.y/10, pos.z/10));
			model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
			model = glm::scale(model, glm::vec3(scale));

			glm::mat4 transformMatrix = projection * view * model;
			shaderProg.set("model", model);
			shaderProg.set("transform", transformMatrix);
			shaderProg.set("lightPos", lightPos);
			shaderProg.set("lightColor", lightColor);
			shaderProg.set("objectColor", glm::vec3(102.0f/256.0f, 1.0f, 1.0f));
			shaderProg.set("viewPos", cameraPos);
			shaderProg.set("specular", specular);
		});  

		lightCube.draw(pointLightShader, [&projection, &view, lightPos=lightPosition, lightColor=lightColor] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3{lightPos.x/20, lightPos.y/20, lightPos.z/20});
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

			glm::mat4 transformMatrix = projection * view * model;
			shaderProg.set("transform", transformMatrix);
			shaderProg.set("objectColor", lightColor);
		});

		std::cout << "Camera: ";
		std::cout << camera.Position.x << " "
				  << camera.Position.y << " "
				  << camera.Position.z << " "
				  << std::endl;
		std::cout << "Light: ";
		std::cout << lightPosition.x << " "
				  << lightPosition.y << " "
				  << lightPosition.z << " "
				  << std::endl;
	}

	void End() override {
		
	}
};

int main(int argc, char *argv[])
{
	App app {};
	app.Run();
}