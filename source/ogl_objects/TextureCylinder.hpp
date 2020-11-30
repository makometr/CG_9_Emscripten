#pragma once
#include "AbstractOpenGLObject.hpp"

class TextureCylinder : public AbstractOpenGLObject {
public:
	TextureCylinder(float radiusDown, float radiusUp) : radiusDown(radiusDown), radiusUp(radiusUp) {};
	~TextureCylinder() = default;

	virtual void initBuffers() { 
        std::vector<GLfloat> vertices;

        float angleStepSize = 360 / pointsCircleNum;
        // float stepTextureCoord = 1.0f / pointsCircleNum;
        // Bottom
        for (int i = 0; i < pointsCircleNum; i++){
            float angle = i*angleStepSize;
        
            vertices.insert(vertices.end(), {
                radiusDown*glm::cos(glm::radians(angle)), -0.5f, radiusDown*glm::sin(glm::radians(angle)), // Pos
                0.0f, -1.0f, 0.0f, // Normal
                1.0f, angle/360.0f, // Tex
            });
            auto middle = {
                0.0f, -0.5f, 0.0f, // Pos
                0.0f, -1.0f, 0.0f, // Normal
                0.0f, 0.0f, // Tex
            };
            vertices.insert(vertices.end(), middle);
            angle += angleStepSize;
            vertices.insert(vertices.end(), {
                radiusDown*glm::cos(glm::radians(angle)), -0.5f, radiusDown*glm::sin(glm::radians(angle)), // Pos
                0.0f, -1.0f, 0.0f, // Normal
                1.0f, angle/360.0f, // Tex
            });

            if (i == pointsCircleNum-1) {
                vertices.insert(vertices.end(), {
                    radiusDown*glm::cos(glm::radians(angle)), -0.5f, radiusDown*glm::sin(glm::radians(angle)), // Pos
                    0.0f, -1.0f, 0.0f, // Normal
                    1.0f, angle/360.0f, // Tex
                }); 
                vertices.insert(vertices.end(), middle);
                vertices.insert(vertices.end(), {
                    radiusDown, -0.5f, 0.0f, // Pos
                    0.0f, -1.0f, 0.0f, // Normal
                    1.0f, 1.0f, // Tex
                });
            }
        }
        // Top
        for (int i = 0; i < pointsCircleNum; i++){
            float angle = i*angleStepSize;
        
            vertices.insert(vertices.end(), {
                radiusUp*glm::cos(glm::radians(angle)), 0.5f, radiusUp*glm::sin(glm::radians(angle)), // Pos
                0.0f, -1.0f, 0.0f, // Normal
                1.0f, angle/360.0f, // Tex
            });
            auto middle = {
                0.0f, 0.5f, 0.0f, // Pos
                0.0f, 1.0f, 0.0f, // Normal
                0.0f, 0.0f, // Tex
            };
            vertices.insert(vertices.end(), middle);
            angle += angleStepSize;
            vertices.insert(vertices.end(), {
                radiusUp*glm::cos(glm::radians(angle)), 0.5f, radiusUp*glm::sin(glm::radians(angle)), // Pos
                0.0f, 1.0f, 0.0f, // Normal
                1.0f, angle/360.0f, // Tex
            });

            if (i == pointsCircleNum-1) {
                vertices.insert(vertices.end(), {
                    radiusUp*glm::cos(glm::radians(angle)), 0.5f, radiusUp*glm::sin(glm::radians(angle)), // Pos
                    0.0f, 1.0f, 0.0f, // Normal
                    1.0f, angle/360.0f, // Tex
                }); 
                vertices.insert(vertices.end(), middle);
                vertices.insert(vertices.end(), {
                    radiusUp, 0.5f, 0.0f, // Pos
                    0.0f, 1.0f, 0.0f, // Normal
                    1.0f, 1.0f, // Tex
                });
            }
        }
        // Side
        for (int i = 0; i < pointsCircleNum; i++){
            float angle = i*angleStepSize;
            float nextAngle = angle + angleStepSize;

            auto lt = {
                radiusUp*glm::cos(glm::radians(angle)), 0.5f, radiusUp*glm::sin(glm::radians(angle)), // Pos
                radiusUp*glm::cos(glm::radians(angle)), 0.0f, radiusUp*glm::sin(glm::radians(angle)), // Normal
                1.0f, angle/360.0f, // Tex
            };
            auto rt = {
                radiusUp*glm::cos(glm::radians(nextAngle)), 0.5f, radiusUp*glm::sin(glm::radians(nextAngle)), // Pos
                radiusUp*glm::cos(glm::radians(nextAngle)), 0.0f, radiusUp*glm::sin(glm::radians(nextAngle)), // Normal
                1.0f, nextAngle/360.0f, // Tex
            };

            auto lb = {
                radiusDown*glm::cos(glm::radians(angle)), -0.5f, radiusDown*glm::sin(glm::radians(angle)), // Pos
                radiusDown*glm::cos(glm::radians(angle)), 0.0f, radiusDown*glm::sin(glm::radians(angle)), // Normal
                0.0f, angle/360.0f, // Tex
            };
            auto rb = {
                radiusDown*glm::cos(glm::radians(nextAngle)), -0.5f, radiusDown*glm::sin(glm::radians(nextAngle)), // Pos
                radiusDown*glm::cos(glm::radians(nextAngle)), 0.0f, radiusDown*glm::sin(glm::radians(nextAngle)), // Normal
                0.0f, nextAngle/360.0f, // Tex
            };

            // First triangle
            vertices.insert(vertices.end(), lt);
            vertices.insert(vertices.end(), rt);
            vertices.insert(vertices.end(), lb);
            // Second
            vertices.insert(vertices.end(), rt);
            vertices.insert(vertices.end(), lb);
            vertices.insert(vertices.end(), rb);

            if (i == pointsCircleNum-1) {
                vertices.insert(vertices.end(), rt);
                vertices.insert(vertices.end(), rb);
                vertices.insert(vertices.end(), {
                    radiusDown, -0.5f, 0.0f, // Pos
                    radiusDown, 0.0f, 0.0f, // Normal
                    0.0f, 1.0f, // Tex
                });

                vertices.insert(vertices.end(), rt);
                vertices.insert(vertices.end(), {
                    radiusUp, 0.5f, 0.0f, // Pos
                    radiusUp, 0.0f, 0.0f, // Normal
                    0.0f, 1.0f, // Tex
                });
                vertices.insert(vertices.end(), {
                    radiusDown, -0.5f, 0.0f, // Pos
                    radiusDown, 0.0f, 0.0f, // Normal
                    0.0f, 1.0f, // Tex
                });
            }
        }
        len = std::size(vertices);
		GLuint VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, std::size(vertices)*sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

		glBindVertexArray(VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);	
	}
	
    virtual void draw(const Shader& shaderProg, std::function<void(const Shader& shaderProg)> shaderDataApplayer) const {
        shaderProg.use();
        shaderDataApplayer(shaderProg);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, len);
        glBindVertexArray(0);
    }
private:
    const int pointsCircleNum = 30;
    GLuint len;
    float radiusDown, radiusUp;
};