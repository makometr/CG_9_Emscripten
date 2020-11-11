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



// class Mesh {
// public:
// 	// void draw(GLuint shaderProgram) {
// 	// 	glUseProgram(shaderProgram);
// 	void draw() {
// 		glUseProgram(shaderProgram);
// 		glBindVertexArray(vao);
// 		glDrawElements(GL_TRIANGLES, vertsNumber, GL_UNSIGNED_INT, static_cast<void*>(0));		
// 		glBindVertexArray(0);
// 	}
// private:
// 	GLuint vao, vertsNumber, shaderProgram;
// };


class App : public BaseApp {

	std::array<float, 6*6*3*2> vertices = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
	};

	std::array<Vertex, 4> rect_vs = {
		Vertex{{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		Vertex{{-0.5f, 0.5f, 0.0f}, {0.5f, 0.0f, 1.0f}}
	};

	GLuint indices[6] = {  // Помните, что мы начинаем с 0!
		0, 1, 3,   // Первый треугольник
		1, 2, 3    // Второй треугольник	
	};
	std::array<float, 8*3> vertices_cube = {
        // Coordinates
        -0.5,  0.5, 0.5, // N-W
         0.5,  0.5, 0.5, // N-E
         0.5, -0.5, 0.5, // S-E
        -0.5, -0.5, 0.5, // S-W

        -0.5,   0.5,  -0.5,   // N-W
         0.5,   0.5,  -0.5,   // N-E
         0.5,  -0.5,  -0.5,   // S-E
        -0.5,  -0.5,  -0.5,   // S-W
    };
    std::array<GLuint, 12*2> indices_cube = {
        0,1, 1,2, 2,3, 3,0,
        0,4, 4,5, 5,1,
        4,7, 7,6, 6,5,
        6,2, 7,3 
    };
	// GLuint VAO_cube, VBO_cube, EBO_cube;
	GLuint VAO = 0, VBO = 0;
	
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


	void Start() override {
		glEnable(GL_DEPTH_TEST);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cmcbManager.setCallbacks(window, &camera);

		ImGui::StyleColorsLight();
		glClearColor(1.0, 0.87, 0.83, 1.0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO); // Vertex Buffer Object
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		axes.initBuffers();
		lightCube.initBuffers();
		figure_cube.initBuffers();

		// glGenVertexArrays(1, &VAO_cube);
		// glGenBuffers(1, &VBO_cube);
		// glGenBuffers(1, &EBO_cube);
		// glBindVertexArray(VAO_cube);

		// glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
		// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube.data(), GL_STATIC_DRAW);

		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_cube), indices_cube.data(), GL_STATIC_DRAW);

		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
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

		ImGui::Begin("Triangle");
		ImGui::SliderFloat("Speed", &speed, -100.0, 100.0);
		ImGui::SliderFloat("Translate: X", &position.x, -100.0, 100.0);
		ImGui::SliderFloat("Translate: Y", &position.y, -100.0, 100.0);
		ImGui::SliderFloat("Translate: Z", &position.z, -100.0, 100.0);
		ImGui::SliderFloat("X Rotate", &rotate.x, -360.0, 360.0);
		ImGui::SliderFloat("Y Rotate", &rotate.y, -360.0, 360.0);
		ImGui::SliderFloat("Z Rotate", &rotate.z, -360.0, 360.0);
		ImGui::SliderFloat("Scale", &scale_tmp, -5.0, 5.0);
		ImGui::SliderFloat3("Light Color", glm::value_ptr(tmp_light_color), 0, 1);

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

		// glBindVertexArray(VAO);
		// glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		// glBindVertexArray(0); 


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
		figure_cube.draw(lightedObjectShader, [&projection, &view, &rotate, position=position, scale_tmp=scale_tmp, lightPos=lightPos] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3(position.x/10, position.y/10, position.z/10));
			model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
			model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
			model = glm::scale(model, glm::vec3(scale_tmp));

			glm::mat4 transformMatrix = projection * view * model;
			GLuint modelLoc = glGetUniformLocation(shaderProg.getId(), "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			GLuint transformLoc = glGetUniformLocation(shaderProg.getId(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));
			GLint lightPosLoc = glGetUniformLocation(shaderProg.getId(), "lightPos");
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
			GLint lightColorPosLoc = glGetUniformLocation(shaderProg.getId(), "lightColor");
			glUniform3fv(lightColorPosLoc, 1, glm::value_ptr(glm::vec3(1.0f)));
			GLint objectColorLoc = glGetUniformLocation(shaderProg.getId(), "objectColor");
			glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
		});

		lightCube.draw(pointLightShader, [&projection, &view] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
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