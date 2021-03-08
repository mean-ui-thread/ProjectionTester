#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>

#include "ShaderProgram.h"

struct VertexBuffer
{
    enum Hint
    {
        Stream  = GL_STREAM_DRAW,
        Static  = GL_STATIC_DRAW,
        Dynamic = GL_DYNAMIC_DRAW
    };

    GLuint handle = 0;

    VertexBuffer()
    {
        glGenBuffers(1, &handle);
    }

    ~VertexBuffer()
    {
        glDeleteBuffers(1, &handle);
        handle = 0;
    }

    void bind(ShaderProgram *program)
    {
        glBindBuffer(GL_ARRAY_BUFFER, handle);

        for(size_t i = 0; i < program->attributeLocations.size(); ++i)
        {
            glVertexAttribPointer(program->attributeLocations[i], program->attributes[i].count, program->attributes[i].type, program->attributes[i].normalize, program->vertexSize,  reinterpret_cast<const GLvoid*>(program->attributeOffsets[i]));
        }

    }

    void unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename Vertex>
    void upload(const std::vector<Vertex> &vertices, Hint hint)
    {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), hint);
    }

};

#endif // VERTEXBUFFER_H
