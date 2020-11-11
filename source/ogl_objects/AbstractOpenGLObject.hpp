#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <functional>

#include "../Shader.hpp"


class AbstractOpenGLObject {
public:
    AbstractOpenGLObject() = default;
    virtual ~AbstractOpenGLObject() = default;

    virtual void initBuffers() = 0;
    virtual void draw(const Shader& shaderProg, std::function<void(const Shader& shaderProg)> shaderDataApplayer) const = 0;

protected:
	GLuint VAO;
};