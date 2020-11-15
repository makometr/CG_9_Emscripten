#pragma once

#include <iostream>
#include <map>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

enum class MaterialType {
    GOLD, cyanPlastic, Emerlad
};

class Material {
public:
	Material (glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, GLfloat shininess) :
		ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)  {}

	const glm::vec3& getAmbient() const { return ambient; };
	const glm::vec3& getDiffuse() const { return diffuse; };
	const glm::vec3& getSpecular() const { return specular; };
	GLfloat getShininess() const { return shininess; };

private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
};


class Materials {
public:
    Materials() = default;
    ~Materials() = default;

    const Material& getMaterial(MaterialType matType) const {
        return materials.at(matType);
    }



private:
    inline static const std::map<MaterialType, Material>
    materials {
                { MaterialType::GOLD, {
                    {0.24725f, 0.1995f, 0.0745f},
                    {0.75164f, 0.60648f, 0.22648f},
                    {0.628281f, 0.555802f, 0.366065f},
                    0.4f}
                },
                { MaterialType::cyanPlastic, {
                    {0.0f, 0.1f, 0.06f},
                    {0.0f, 0.50980392f, 0.50980392f},
                    {0.50196078f, 0.50196078f, 0.50196078f},
                    32.0f}
                },
                { MaterialType::Emerlad, {
                    {0.0215, 0.1745, 0.0215},
                    {0.07568, 0.61424, 0.07568},
                    {0.633, 0.727811, 0.633},
                    0.6f}
                },
    };
};