#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glad/glad.h>
#include <SDL2/SDL_image.h>

struct Texture
{
    GLuint handle = 0;
    std::string filePath;
    int width = 0;
    int height = 0;

    Texture(const std::string &filePath) : filePath(filePath)
    {
        glGenTextures(1, &handle);
    }

    ~Texture()
    {
        glDeleteTextures(1, &handle);
        handle = 0;
    }

    int decode()
    {
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if(surface == NULL)
        {
            SDL_LogCritical(0, "Unable to load image %s: %s", filePath.c_str(), IMG_GetError());
            return -1;
        }

        SDL_Surface *surfaceRGBA = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
        if (surfaceRGBA == NULL)
        {
            SDL_LogCritical(0, "Unable to load convert %s to RGBA: %s", filePath.c_str(), SDL_GetError());
            SDL_FreeSurface(surface);
            return -1;
        }

        width = surfaceRGBA->w;
        height = surfaceRGBA->h;

        bind(0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceRGBA->w, surfaceRGBA->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceRGBA->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        SDL_FreeSurface(surfaceRGBA);
        SDL_FreeSurface(surface);

        return 0;
    }

    void bind(GLuint textureSlot = 0)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, handle);
    }

    void unbind(GLuint textureSlot = 0)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

#endif // TEXTURE_H
