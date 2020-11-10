#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>

#include <array>
#include <algorithm>
#include <iostream>

#include "../Shader.hpp"

extern const GLuint WIDTH;
extern const GLuint HEIGHT;

class Axes {
public:
	Axes() = default;
	~Axes() = default;

	void initBuffers() {
		std::array<float, 6*(3+3)> vertices = {
			-1.0,  0.0,  0.0, 1.0,  0.0,  0.0,
			1.0,  0.0,  0.0, 1.0,  0.0,  0.0,
			0.0, -1.0,  0.0, 0.0,  1.0,  0.0,
			0.0,  1.0,  0.0, 0.0,  1.0,  0.0,
			0.0,  0.0, -1.0, 0.0,  0.0,  1.0,
			0.0,  0.0,  1.0, 0.0,  0.0,  1.0,
    	};
		GLuint VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);	
	}

	void updateTransfrom(const glm::mat4& view, int e) {
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 100));
		glm::mat4 projection {1.0f};
		if (!e)
        	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		else
			projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f );
		transformMatrix = projection * view * model;
	}
	
	void draw(const Shader& shderProgramm) const {
		GLuint transformLoc = glGetUniformLocation(shderProgramm.getId(), "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));

		glBindVertexArray(VAO);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 6*3);
		glBindVertexArray(0);
	}

private:
	GLuint VAO;
	glm::mat4 transformMatrix {1.0f};
};