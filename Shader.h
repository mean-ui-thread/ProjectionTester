#ifndef SHADER_H
#define SHADER_H

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <SDL2/SDL_log.h>

struct Shader
{
    GLuint handle = 0;
    std::string filePath;

    Shader(const std::string &filePath) : filePath(filePath)
    {
        std::string ext = filePath.substr(filePath.find_last_of(".") + 1);

        if (ext == "vert" || ext == "vsh")
        {
            handle = glCreateShader(GL_VERTEX_SHADER);
        }
        else if (ext == "frag" || ext == "fsh")
        {
            handle = glCreateShader(GL_FRAGMENT_SHADER);
        }
    }

    ~Shader() {
        if (handle)
        {
            glDeleteShader(handle);
            handle = 0;
        }
    }

    int compile()
    {
        assert(handle);

        std::ifstream f;
        f.open(filePath);
        if (!f.is_open())
        {
            SDL_LogCritical(0, "Could not open %s", filePath.c_str());
            return -1;
        }

        std::stringstream shaderStream;
        shaderStream << f.rdbuf();

        std::string sourceCode = shaderStream.str();

        f.close();

        const char * rawSourceCode = sourceCode.c_str();

        glShaderSource(handle, 1, &rawSourceCode, NULL);
        glCompileShader(handle);

        GLint compileStatus;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);

        if (!compileStatus)
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(handle, sizeof(infoLog), NULL, infoLog);
            SDL_LogCritical(0, "Could not compile %s : %s", filePath.c_str(), infoLog);
            return -1;
        }

        return 0;
    }
};

#endif // SHADER_H
