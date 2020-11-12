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
	Shader basicShader{"resources/shaders/basic.vs", "resources/shaders/basic.fs"};
	Shader axesShader{"resources/shaders/axes.vs", "resources/shaders/basic.fs"};
	Shader pointLightShader{"resources/shaders/point_light.vs", "resources/shaders/point_light.fs"};
	Shader lightedObjectShader{"resources/shaders/lighted.vs", "resources/shaders/lighted.fs"};

	Axes axes {};

	StandardCube lightCube {};
	StandardCube figure_cube {};

	glm::vec3 position {0.0f};
	glm::vec3 rotate {0.0f};
	glm::vec3 scale {0.0f};
	float speed = 1;
	float scale_tmp = 1;

	CameraMoveCallbackManager cmcbManager {};
	Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
	GLfloat currentFrame;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	glm::vec3 tmp_light_color {1.0f};
	glm::vec3 lightPos {0.0f, 2.0f, 0.0f};
	float specular;

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

		ImGui::SetNextWindowSize({300,400}, ImGuiCond_Once);

		ImGui::Begin("Light");
		ImGui::SliderFloat("Speed", &speed, -100.0, 100.0);
		ImGui::SliderFloat("Translate: X", &lightPos.x, -100.0, 100.0);
		ImGui::SliderFloat("Translate: Y", &lightPos.y, -100.0, 100.0);
		ImGui::SliderFloat("Translate: Z", &lightPos.z, -100.0, 100.0);
		ImGui::SliderFloat("X Rotate", &rotate.x, -360.0, 360.0);
		ImGui::SliderFloat("Y Rotate", &rotate.y, -360.0, 360.0);
		ImGui::SliderFloat("Z Rotate", &rotate.z, -360.0, 360.0);
		ImGui::SliderFloat("Scale", &scale_tmp, -5.0, 5.0);
		ImGui::SliderFloat3("Light Color", glm::value_ptr(tmp_light_color), 0, 1);
		ImGui::SliderFloat("Specular###IDFORSLIDER", &specular, 0.001, 256.0);

		static int ProjectionType = 0;
        ImGui::RadioButton("Perspective", &ProjectionType, 0);
		ImGui::SameLine();
        ImGui::RadioButton("Orthographic", &ProjectionType, 1);
		ImGui::End();

		// ImGui::ShowDemoWindow();

		basicShader.use();
		// передаем в шейдер цвет источника света
		GLint lightColorLoc = glGetUniformLocation(basicShader.getId(), "lightColor");
		glUniform3f(lightColorLoc, tmp_light_color.r, tmp_light_color.g, tmp_light_color.b);

		glm::mat4 model {1.0f}; 
		model = glm::translate(model, glm::vec3(position.x/10, position.y/10, position.z/10));
		model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(scale_tmp));

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection {1.0f};
		if (!ProjectionType)
        	projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		else
			projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f );

		glm::mat4 result = projection * view * model;
		GLuint transformLoc = glGetUniformLocation(basicShader.getId(), "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(result)); 



		static const std::array<glm::vec3, 3> positions = {
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(0.0f, 2.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
		};
		static const std::array<glm::vec3, 3> colors = {
			glm::vec3{101.0f, 210.0f, 69.0f}/256.0f,
			glm::vec3{31.0f, 171.0f, 205.0f}/256.0f,
			glm::vec3{254.0f, 200.0f, 47.0f}/256.0f
		};
		
		static glm::vec3 scales {3.0f, 1.0f, 2.0f};
		scales *= scale_tmp;
		// for (size_t i {1}; i <= 3; i++) {
		// 	glm::mat4 model {1.0f};
		// 	model = glm::translate(model, positions[i-1]);
		// 	model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
		// 	model = glm::scale(model, glm::vec3(scales));
		// 	glm::mat4 result = projection * view * model;

		// 	GLuint transformLoc = glGetUniformLocation(basicShader.getId(), "transform");
		// 	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(result));

		// 	GLuint transformLoc_color = glGetUniformLocation(basicShader.getId(), "color");
		// 	glUniform3fv(transformLoc_color, 1, glm::value_ptr(colors[i-1]));

		// 	glDrawArrays(GL_TRIANGLES, 0, 36);
		// }
		glBindVertexArray(0);

		// glLineWidth(2.0f); 
		// glBindVertexArray(VAO_cube);
		// glDrawElements(GL_LINES, 4*6, GL_UNSIGNED_INT, 0);


		axes.draw(axesShader, [&projection, &view] (const Shader& shaderProg) {
			glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100));
			glm::mat4 transformMatrix = projection * view * model;
			GLuint transformLoc = glGetUniformLocation(shaderProg.getId(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		});

		glm::vec3 rotate = glm::vec3((GLfloat)(int(glfwGetTime() * speed) % 360));
		figure_cube.draw(lightedObjectShader, [&projection, &view, &rotate, cameraPos=camera.Position, position=position, scale_tmp=scale_tmp, lightPos=lightPos, specular=specular] (const Shader& shaderProg) {
			glm::mat4 model {1.0f}; 
			model = glm::translate(model, glm::vec3(position.x/10, position.y/10, position.z/10));
			model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
			model = glm::scale(model, glm::vec3(scale_tmp));

			glm::mat4 transformMatrix = projection * view * model;
			shaderProg.set("model", model);
			shaderProg.set("transform", transformMatrix);
			shaderProg.set("lightPos", lightPos);
			shaderProg.set("lightColor", glm::vec3(1.0f));
			shaderProg.set("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
			shaderProg.set("viewPos", cameraPos);
			shaderProg.set("specular", specular);
		});  

		lightCube.draw(pointLightShader, [&projection, &view, lightPos=lightPos] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3{lightPos.x/20, lightPos.y/20, lightPos.z/20});
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

			glm::mat4 transformMatrix = projection * view * model;
			GLuint transformLoc = glGetUniformLocation(shaderProg.getId(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		});
	}

	void End() override {
		
	}
};

int main(int argc, char *argv[])
{
	App app {};
	app.Run();
}