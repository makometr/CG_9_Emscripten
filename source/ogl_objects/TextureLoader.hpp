#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"




class TextureLoader {
public:
    TextureLoader() = delete;


    static std::pair<unsigned, bool> loadTexture(const std::string &path) {
        std::cout << path << std::endl;
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
		auto image_deleter = [](unsigned char* data) {
			stbi_image_free(data);
		};
		std::unique_ptr<unsigned char, decltype(image_deleter)> data {(unsigned char*)stbi_load(path.c_str(), &width, &height, &nrComponents, 0), image_deleter};

        if (!data)
            return { {}, false};

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.get());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return {textureID, true};
    } 

private:

};

