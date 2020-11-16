#pragma once

#include "AbstractOpenGLObject.hpp"


class Axes : AbstractOpenGLObject {
public:
	Axes() = default;
	virtual ~Axes() = default;

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
	
	virtual void draw(const Shader& shaderProg, std::function<void(const Shader& shaderProg)> shaderDataApplayer) const {
		shaderProg.use();
		shaderDataApplayer(shaderProg);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 6*3);
		glBindVertexArray(0);
	}
};