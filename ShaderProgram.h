#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "AttributeInfo.h"
#include "Shader.h"

struct ShaderProgram
{
    GLuint handle = 0;
    std::vector<AttributeInfo> attributes;
    std::vector<GLint> attributeLocations;
    std::vector<size_t> attributeOffsets;
    GLint vertexSize = 0;

    ShaderProgram(const std::vector<AttributeInfo> &attributes) :
          attributes(attributes)
    {
        attributeLocations.resize(attributes.size(), -1);
        attributeOffsets.resize(attributes.size(), 0);
        handle = glCreateProgram();
    }

    ~ShaderProgram()
    {
        if (handle)
        {
            glDeleteProgram(handle);
            handle = 0;
        }
    }

    void attach(Shader *shader)
    {
        glAttachShader(handle, shader->handle);
    }

    int link()
    {
        glLinkProgram(handle);

        GLint linkStatus;
        glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus)
        {
            GLchar infoLog[1024];
            glGetProgramInfoLog(handle, sizeof(infoLog), NULL, infoLog);
            SDL_LogCritical(0, "Could not link shader program:\n%s", infoLog);
            return -1;
        }

        vertexSize = 0;
        for(size_t i = 0; i < attributes.size(); ++i)
        {
            attributeLocations[i] = glGetAttribLocation(handle, attributes[i].name.c_str());
            if (attributeLocations[i] == -1)
            {
                SDL_LogCritical(0, "Could not find attribute named \"%s\" in shader program.", attributes[i].name.c_str());
                return -1;
            }

            attributeOffsets[i] = vertexSize;
            vertexSize += attributes[i].count * attributes[i].sizeOfType();
        }


        return 0;
    }

    void bind()
    {
        glUseProgram(handle);
        for(size_t i = 0; i < attributeLocations.size(); ++i)
        {
            glEnableVertexAttribArray(attributeLocations[i]);
        }
    }

    void unbind()
    {
        for(size_t i = 0; i < attributeLocations.size(); ++i)
        {
            glDisableVertexAttribArray(attributeLocations[i]);
        }
        glUseProgram(0);
    }

    GLint getUniformLocation(const char *uniformName)
    {
        GLint location = glGetUniformLocation(handle, uniformName);
        if (location == -1)
        {
            SDL_LogCritical(0, "Could not find uniform named \"%s\" in shader program.", uniformName);
        }
        return location;
    }

    void setUniform(int32_t uniformLocation, float value)
    {
        glUniform1f(uniformLocation, value);
    }

    void setUniform(int32_t uniformLocation, const glm::vec2 &value)
    {
        glUniform2f(uniformLocation, value.x, value.y);
    }

    void setUniform(int32_t uniformLocation, const glm::vec3 &value)
    {
        glUniform3f(uniformLocation, value.x, value.y, value.z);
    }

    void setUniform(int32_t uniformLocation, const glm::vec4 &value)
    {
        glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
    }

    void setUniform(int32_t uniformLocation, int32_t value)
    {
        glUniform1i(uniformLocation, value);
    }

    void setUniform(int32_t uniformLocation, const glm::ivec2 &value)
    {
        glUniform2i(uniformLocation, value.x, value.y);
    }

    void setUniform(int32_t uniformLocation, const glm::ivec3 &value)
    {
        glUniform3i(uniformLocation, value.x, value.y, value.z);
    }

    void setUniform(int32_t uniformLocation, const glm::ivec4 &value)
    {
        glUniform4i(uniformLocation, value.x, value.y, value.z, value.w);
    }

    void setUniform(int32_t uniformLocation, const glm::mat2 &value)
    {
        glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(int32_t uniformLocation, const glm::mat3 &value)
    {
        glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(int32_t uniformLocation, const glm::mat4 &value)
    {
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(int32_t uniformLocation, const std::vector<float> &values)
    {
        glUniform1fv(uniformLocation, (GLsizei)values.size(), values.data());
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::vec2> &values)
    {
        glUniform2fv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::vec3> &values)
    {
        glUniform3fv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::vec4> &values)
    {
        glUniform4fv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<int32_t> &values)
    {
        glUniform1iv(uniformLocation, (GLsizei)values.size(), values.data());
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::ivec2> &values)
    {
        glUniform2iv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::ivec3> &values)
    {
        glUniform3iv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::ivec4> &values)
    {
        glUniform4iv(uniformLocation, (GLsizei)values.size(), glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::mat2> &values)
    {
        glUniformMatrix2fv(uniformLocation, (GLsizei)values.size(), GL_FALSE, glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::mat3> &values)
    {
        glUniformMatrix3fv(uniformLocation, (GLsizei)values.size(), GL_FALSE, glm::value_ptr(values.front()));
    }

    void setUniform(int32_t uniformLocation, const std::vector<glm::mat4> &values)
    {
        glUniformMatrix4fv(uniformLocation, (GLsizei)values.size(), GL_FALSE, glm::value_ptr(values.front()));
    }
};

#endif // SHADERPROGRAM_H
