#pragma once
#include <glad/glad.h>
#include <string>
#include <optional>
#include <glm/glm.hpp>

#include "Materials.hpp"


class Shader
{
public:
	Shader(const std::string &vPath, const std::string &fPath);
	Shader(const Shader &shader) = delete;
	// Shader(const std::string& vPath, const std::string& fPath, const std::string& gPath);
	void use() const;
	GLuint getId() const;

	void set(const std::string& name, const glm::vec3& value) const;
	void set(const std::string& name, const glm::mat4& value) const;
	void set(const std::string& name, float value) const;
	void set(const std::string& name, bool value) const;
	void setMaterial(const std::string& name, const Material& material) const;

private:
	static std::optional<std::string> loadFromFile(const std::string &path);

private:
	std::string vPath, fPath, gPath;
	std::string vSource, fSource, gSource;
	GLuint id;
};