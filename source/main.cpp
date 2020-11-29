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
#include <memory>
#include <future>

#include <array>
#include <algorithm>
#include <iostream>
#include <map>

#include "Materials.hpp"
#include "Camera.hpp"
#include "CameraMoveCallbackManager.hpp"
#include "ogl_objects/TextureLoader.hpp"
#include "ogl_objects/AxesOpenGL.hpp"
#include "ogl_objects/StandardCube.hpp"
#include "ogl_objects/TextureCube.hpp"



const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

void initShadowMapping(GLuint& FBO, GLuint& depthTexture) {
	glGenFramebuffers(1, &FBO);  
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	// glDrawBuffers(0, nullptr);
	// glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

glm::mat4 genModel(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
	glm::mat4 model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale);
	return model;
}


// Window dimensions
// TODO move to state 
constexpr GLuint WIDTH = 800, HEIGHT = 600;
static bool isAxesEnabled = true;
static std::array<bool, 2> pointLightsTurned {true, true};

Materials materialManager {};
std::reference_wrapper<const Material> curMat = materialManager.getMaterial(MaterialType::GOLD);

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

struct DrawablePair {
	AbstractOpenGLObject &obj;
	glm::mat4 model;
};

class App : public BaseApp {
	// Shader basicShader{"resources/shaders/basic.vs", "resources/shaders/basic.fs"};
	Shader axesShader{"resources/shaders/axes.vs", "resources/shaders/axes.fs"};
	Shader pointLightShader{"resources/shaders/point_light.vs", "resources/shaders/point_light.fs"};
	Shader lightedTexturedObjectShader{"resources/shaders/texturedCube.vs", "resources/shaders/texturedCube.fs"};
	Shader shadowMapShader{"resources/shaders/shadow.vs", "resources/shaders/shadow.fs"};


	Axes axes {};
	StandardCube lightCube {};
	TextureCube figure_cube {};

	CameraMoveCallbackManager cmcbManager {};
	Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};

	// For camera
	GLfloat currentFrame;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// Cube state
	float cubeRotateSpeed = 0;
	glm::vec3 cubePosition {0.0f};
	glm::vec3 cubeRotate {0.0f};
	GLfloat cubeRotateValue {0.0f};
	// glm::vec3 cubeScale {0.0f};
	GLfloat cubeScale {1.0f};

	// Light state
	glm::vec3 pointLightPosition_1 {4.0f, 4.0f, 0.0f};
	glm::vec3 pointLightColor_1 {1.0f};
	glm::vec3 pointLightPosition_2 {-4.0f, 4.0f, 0.0f};
	glm::vec3 pointLightColor_2 {1.0f};
	// float specular {128.0f};

	// textures
	unsigned diffuseTexture;
	unsigned specularTexture;

	GLuint depthMap;
	GLuint depthMapFBO;

	std::vector<DrawablePair> drawables;


	void Start() override {
		auto texture_load_result_1 = std::async(std::launch::deferred,TextureLoader::loadTexture, "resources/textures/container2.png");
		auto texture_load_result_2 = std::async(std::launch::deferred,TextureLoader::loadTexture, "resources/textures/container2_specular.png");
		glEnable(GL_DEPTH_TEST);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cmcbManager.setCallbacks(window, &camera);

		ImGui::StyleColorsLight();
		glClearColor(1.0, 0.87, 0.83, 1.0);


		axes.initBuffers();
		lightCube.initBuffers();
		figure_cube.initBuffers();

		auto [id_1, status_1] = texture_load_result_1.get();
		auto [id_2, status_2] = texture_load_result_2.get();
		if (!status_1 || !status_2) {
			std::cout << "Texture failed to load!" << std::endl;
			exit(0); // TODO handle
		}
		diffuseTexture = id_1;
		specularTexture = id_2;
		initShadowMapping(depthMapFBO, depthMap);

		glm::mat4 model {1.0f}; 
		drawables.push_back({figure_cube, genModel(cubePosition, cubeRotate, glm::vec3{1.0f, 3.0f, 1.0f})});
		model = glm::mat4{1.0f};
		model = glm::translate(model, glm::vec3(cubePosition.x+1.5f, cubePosition.y-1.0f, cubePosition.z));
		model = glm::rotate(model, glm::radians(cubeRotate.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(cubeRotate.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(cubeRotate.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
		drawables.push_back({figure_cube, model});
		// "Plane"
		model = glm::mat4{1.0f}; 
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 0.1f, 30.0f));
		drawables.push_back({figure_cube, model});
	}

	void Update(float dTime) override {
        // Set frame time
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 
		if (cmcbManager.getCameraActiveStatus())
			cmcbManager.applyPlayerMoveControllerChanges(deltaTime);

		glfwPollEvents();

		ImGui::SetNextWindowSize({300, 80}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Camera");
			static int ProjectionType = 0;
			ImGui::RadioButton("Perspective", &ProjectionType, 0); ImGui::SameLine();
			ImGui::RadioButton("Orthographic", &ProjectionType, 1);
			ImGui::Checkbox("Axes enabled", &isAxesEnabled);
		ImGui::End();


		ImGui::SetNextWindowSize({300, 150}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({10, 100}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Light-1");
			ImGui::SliderFloat("Translate: X", &pointLightPosition_1.x, -10.0, 10.0);
			ImGui::SliderFloat("Translate: Y", &pointLightPosition_1.y, -10.0, 10.0);
			ImGui::SliderFloat("Translate: Z", &pointLightPosition_1.z, -10.0, 10.0);
			ImGui::SliderFloat3("Light Color", glm::value_ptr(pointLightColor_1), 0, 1);
			ImGui::Checkbox("Light enabled", &pointLightsTurned[0]);
		ImGui::End();

		ImGui::SetNextWindowSize({300, 150}, ImGuiCond_Once);
		ImGui::SetNextWindowPos({10, 260}, ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::Begin("Light-2");
			ImGui::SliderFloat("Translate: X", &pointLightPosition_2.x, -10.0, 10.0);
			ImGui::SliderFloat("Translate: Y", &pointLightPosition_2.y, -10.0, 10.0);
			ImGui::SliderFloat("Translate: Z", &pointLightPosition_2.z, -10.0, 10.0);
			ImGui::SliderFloat3("Light Color", glm::value_ptr(pointLightColor_2), 0, 1);
			ImGui::Checkbox("Light enabled", &pointLightsTurned[1]);
		ImGui::End();

		// ImGui::SetNextWindowSize({300, 170}, ImGuiCond_Once);
		// ImGui::SetNextWindowPos({10, 380}, ImGuiCond_Once);
		// ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		// ImGui::Begin("Cube");
		// 	// ImGui::SliderFloat("Speed", &cubeRotateSpeed, -100.0, 100.0);
		// 	// ImGui::SliderFloat("Translate: X", &cubePosition.x, -10.0, 10.0);
		// 	// ImGui::SliderFloat("Translate: Y", &cubePosition.y, -10.0, 10.0);
		// 	// ImGui::SliderFloat("Translate: Z", &cubePosition.z, -10.0, 10.0);
		// 	ImGui::SliderFloat("X Rotate", &cubeRotate.x, 0.0, 360.0);
		// 	ImGui::SliderFloat("Y Rotate", &cubeRotate.y, 0.0, 360.0);
		// 	ImGui::SliderFloat("Z Rotate", &cubeRotate.z, 0.0, 360.0);
		// 	ImGui::SliderFloat("Scale", &cubeScale, -5.0, 5.0);
		// 	static int materialType = 0;
		// 	ImGui::RadioButton("Gold", &materialType, 0); ImGui::SameLine();
		// 	ImGui::RadioButton("Cyan Plastic", &materialType, 1); ImGui::SameLine();
		// 	ImGui::RadioButton("Emerlad", &materialType, 2);
		// ImGui::End();

		// if (materialType == 0)
		// 	curMat = materialManager.getMaterial(MaterialType::GOLD);
		// if (materialType == 1)
		// 	curMat = materialManager.getMaterial(MaterialType::cyanPlastic);
		// if (materialType == 2)
		// 	curMat = materialManager.getMaterial(MaterialType::Emerlad);


		// Update "Physics"
		cubeRotateValue += (std::sin(glfwGetTime())+1) * cubeRotateSpeed/50;
		if (cubeRotateValue > 360.0f)
			cubeRotateValue -= 360.0f;
		cubeRotate = glm::vec3(cubeRotateValue);

		// First calculate shadows

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		float near_plane = 0.1f, far_plane = 90.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(pointLightPosition_1*5.0f, 
		                          glm::vec3(0.0f), 
		                          glm::vec3(0.0f, 1.0f,  0.0f)); 
		glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
		shadowMapShader.use();
		shadowMapShader.set("lightSpaceMatrix", lightSpaceMatrix);
		shadowMapShader.set("model", glm::mat4{1.0f});

		// Draw all "visible" objects
		
		for (auto &drawable : drawables) {
			drawable.obj.draw(shadowMapShader, [model=drawable.model] (const Shader& shaderProg) {
				shaderProg.set("model", model);
			});
		}

		// Then render scene itself

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		lightedTexturedObjectShader.use();
		lightedTexturedObjectShader.set("view", view);
		lightedTexturedObjectShader.set("projection", projection);
		// lightedTexturedObjectShader.set("transform", transformMatrix);
		// lightedTexturedObjectShader.set("objectColor", glm::vec3(102.0f/256.0f, 1.0f, 1.0f));
		lightedTexturedObjectShader.set("pointLightsTurned[0]", pointLightsTurned[0]);
		lightedTexturedObjectShader.set("pointLightsTurned[1]", pointLightsTurned[1]);

		lightedTexturedObjectShader.set("material.diffuse", 0);
		lightedTexturedObjectShader.set("material.specular", 1);
		lightedTexturedObjectShader.set("material.shininess", 64.0f);


		lightedTexturedObjectShader.set("pointLights[0].position", pointLightPosition_1);
		lightedTexturedObjectShader.set("pointLights[0].ambient", pointLightColor_1 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[0].diffuse", pointLightColor_1 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[0].specular", pointLightColor_1 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[0].constant", 1.0f);
		lightedTexturedObjectShader.set("pointLights[0].linear", 0.14f);
		lightedTexturedObjectShader.set("pointLights[0].quadratic", 0.07f);

		lightedTexturedObjectShader.set("pointLights[1].position", pointLightPosition_2);
		lightedTexturedObjectShader.set("pointLights[1].ambient", pointLightColor_2 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[1].diffuse", pointLightColor_2 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[1].specular", pointLightColor_2 * glm::vec3{1.0f, 1.0f, 1.0f});
		lightedTexturedObjectShader.set("pointLights[1].constant", 1.0f);
		lightedTexturedObjectShader.set("pointLights[1].linear", 0.14f);
		lightedTexturedObjectShader.set("pointLights[1].quadratic", 0.07f);

		lightedTexturedObjectShader.set("lightSpaceMatrix", lightSpaceMatrix);
		lightedTexturedObjectShader.set("shadowMap", 2);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);


		for (auto &drawable : drawables) {
			drawable.obj.draw(lightedTexturedObjectShader, [model=drawable.model] (const Shader& shaderProg) {
				shaderProg.set("model", model);
			});
		}
		// figure_cube.draw(lightedTexturedObjectShader, [&projection, &view, rotate=cubeRotate, cameraPos=camera.Position, pos=cubePosition, scale=cubeScale, lightPos_1=pointLightPosition_1, lightPos_2=pointLightPosition_2, plColor_1=pointLightColor_1, plColor_2=pointLightColor_2, diffTexture=diffuseTexture, specTexture=specularTexture] (const Shader& shaderProg) {
		// figure_cube.draw(lightedTexturedObjectShader, [rotate=cubeRotate, pos=cubePosition, scale=cubeScale] (const Shader& shaderProg) {
		// 	glm::mat4 model {1.0f}; 
		// 	model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
		// 	model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
		// 	model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));

		// 	// glm::mat4 transformMatrix = projection * view * model;
		// 	shaderProg.set("model", model);

		// });

		
		// figure_cube.draw(lightedTexturedObjectShader, [rotate=cubeRotate, pos=cubePosition, scale=cubeScale] (const Shader& shaderProg) {
		// 	glm::mat4 model {1.0f}; 
		// 	model = glm::translate(model, glm::vec3(pos.x+1.5f, pos.y-1.0f, pos.z));
		// 	model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
		// 	model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
		// 	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
		// 	shaderProg.set("model", model);
		// });

		// // "Plane"
		// figure_cube.draw(lightedTexturedObjectShader, [rotate=cubeRotate, pos=cubePosition, scale=cubeScale] (const Shader& shaderProg) {
		// 	glm::mat4 model {1.0f}; 
		// 	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		// 	model = glm::scale(model, glm::vec3(30.0f, 0.1f, 30.0f));
		// 	shaderProg.set("model", model);
		// });

		lightCube.draw(pointLightShader, [&projection, &view, pos=pointLightPosition_1, lightColor=pointLightColor_1] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3{pos.x, pos.y, pos.z});
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

			glm::mat4 transformMatrix = projection * view * model;
			shaderProg.set("transform", transformMatrix);
			shaderProg.set("objectColor", lightColor);
		});

		lightCube.draw(pointLightShader, [&projection, &view, pos=pointLightPosition_2, lightColor=pointLightColor_2] (const Shader& shaderProg) {
			glm::mat4 model {1.0f};
			model = glm::translate(model, glm::vec3{pos.x, pos.y, pos.z});
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

			glm::mat4 transformMatrix = projection * view * model;
			shaderProg.set("transform", transformMatrix);
			shaderProg.set("objectColor", lightColor);
		});

		// std::cout << "Camera: ";
		// std::cout << camera.Position.x << " "
		// 		  << camera.Position.y << " "
		// 		  << camera.Position.z << " "
		// 		  << std::endl;
		// std::cout << "Light: ";
		// std::cout << pointLightPosition_1.x << " "
		// 		  << pointLightPosition_1.y << " "
		// 		  << pointLightPosition_1.z << " "
		// 		  << std::endl;
	}

	void End() override {
		
	}
};

int main(int argc, char *argv[])
{
	App app {};
	app.Run();
}