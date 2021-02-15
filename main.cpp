#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "AttributeInfo.h"
#include "ShaderProgram.h"
#include "BaseApp.h"
#include "Texture.h"
#include "VertexBuffer.h"

struct DemoVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

struct DemoApp : public BaseApp
{
    ShaderProgram *defaultProgram = NULL;
    Texture *backgroundTex = NULL;
    VertexBuffer *backgroundVBO = NULL;
    Texture *mikeTex = NULL;
    VertexBuffer *mikeVBO = NULL;
    bool useOrtho = true;
    float fieldOfView = 45.0f;
    glm::vec3 mikePosition = glm::vec3(0.0f);
    glm::vec3 mikeRotation = glm::vec3(0.0f);
    glm::vec3 mikeScale = glm::vec3(1.0f);
    glm::vec3 mikeCenterPoint = glm::vec3(0.0f);

    std::vector<DemoVertex> backgroundVertices = {
        //{   X       Y       Z  }  { S     T  }
        { {  0.0f,   0.0f,   0.0f}, {0.0f, 0.0f} },
        {   {0.0f, 600.0f,   0.0f}, {0.0f, 1.0f} },
        { {800.0f,   0.0f,   0.0f}, {1.0f, 0.0f} },
        { {800.0f, 600.0f,   0.0f}, {1.0f, 1.0f} },
    };

    std::vector<DemoVertex> mikeVertices = {
        //{   X       Y       Z  }  { S     T  }
        { {  0.0f,   0.0f,   0.0f}, {0.0f, 0.0f} },
        {   {0.0f, 512.0f,   0.0f}, {0.0f, 1.0f} },
        { {512.0f,   0.0f,   0.0f}, {1.0f, 0.0f} },
        { {512.0f, 512.0f,   0.0f}, {1.0f, 1.0f} },
    };

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;

    GLint u_MVP = 0;
    GLint u_texture0 = 0;

    virtual bool init() override
    {
        Shader defaultVert("default.vert");
        if (defaultVert.compile() != 0)
        {
            return false;
        }

        Shader defaultFrag("default.frag");
        if (defaultFrag.compile() != 0)
        {
            return false;
        }

        std::vector<AttributeInfo> defaultAttributes = {
            {"a_position", AttributeInfo::Float, 3},
            {"a_texCoord0", AttributeInfo::Float, 2},
        };

        defaultProgram = new ShaderProgram(defaultAttributes);
        defaultProgram->attach(&defaultVert);
        defaultProgram->attach(&defaultFrag);
        if (defaultProgram->link() != 0)
        {
            return false;
        }

        u_MVP      = defaultProgram->getUniformLocation("u_MVP");
        u_texture0 = defaultProgram->getUniformLocation("u_texture0");

        backgroundTex = new Texture("background.jpg");
        if (backgroundTex->decode() != 0)
        {
            return false;
        }

        backgroundVBO = new VertexBuffer();
        backgroundVBO->upload(backgroundVertices, VertexBuffer::Static);

        mikeTex = new Texture("mike.png");
        if (mikeTex->decode() != 0)
        {
            return false;
        }

        mikeVBO = new VertexBuffer();
        mikeVBO->upload(mikeVertices, VertexBuffer::Static);

        return true;
    }

    virtual void shutdown() override
    {
        delete defaultProgram;
        defaultProgram = NULL;

        delete backgroundTex;
        backgroundTex = NULL;

        delete backgroundVBO;
        backgroundVBO = NULL;

        delete mikeTex;
        mikeTex = NULL;

        delete mikeVBO;
        mikeVBO = NULL;
    }

    virtual void render() override
    {
        if (useOrtho)
        {
            projectionMatrix = glm::ortho(0.0f, (float)displayWidth, (float)displayHeight, 0.0f, -8000.0f, 8000.0f);
            viewMatrix = glm::lookAt(glm::vec3(0,0,1),  // eye
                                     glm::vec3(0,0,0),  // center
                                     glm::vec3(0,1,0)); // up
        }
        else
        {
            float fieldOfViewRad = glm::radians(fieldOfView);
            float cameraDistance = ((float)displayHeight/2.0f) / tan(fieldOfViewRad/2.0f); //724.26407f;
            float aspectRatio = (float)displayWidth / (float)displayHeight;

            projectionMatrix = glm::perspective(fieldOfViewRad, aspectRatio, 0000.0f, 8000.0f);
            viewMatrix = glm::lookAt(glm::vec3(displayWidth*0.5f,displayHeight*0.5f,-cameraDistance), // eye
                                     glm::vec3(displayWidth*0.5f,displayHeight*0.5,0),                // center
                                     glm::vec3(0,-1,0));                                              // up
        }

        // Order matters. We use TRSC (Translate, Rotate, Scale, Center)
        modelMatrix  = glm::translate(glm::identity<glm::mat4>(), mikePosition);
        modelMatrix *= glm::yawPitchRoll(glm::radians(mikeRotation.y), glm::radians(mikeRotation.x), glm::radians(mikeRotation.z));
        modelMatrix  = glm::scale(modelMatrix, mikeScale);
        modelMatrix  = glm::translate(modelMatrix, -mikeCenterPoint);

        defaultProgram->bind();
        defaultProgram->setUniform(u_texture0, 0);

        // Draw the background
        defaultProgram->setUniform(u_MVP, projectionMatrix * viewMatrix); // No Model transforms for the background.
        backgroundVBO->bind(defaultProgram);
        backgroundTex->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)backgroundVertices.size());

        // Draw mike
        defaultProgram->setUniform(u_MVP, projectionMatrix * viewMatrix * modelMatrix); // No Model transforms for the background.
        mikeVBO->bind(defaultProgram);
        mikeTex->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)mikeVertices.size());
    }

    virtual void renderUI() override
    {
        char buf[32];

        ImGui::NewFrame();

        ImGui::Begin("Control Panel");
        ImGui::Checkbox("Use Orthographic Projection", &useOrtho);
        ImGui::SliderFloat("Field of View", &fieldOfView, 0, 180);

        if (ImGui::TreeNode("Projection Matrix Viewer"))
        {
            if (ImGui::BeginTable("Projection", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        snprintf(buf, sizeof(buf), "%f", projectionMatrix[row][column]);
                        ImGui::TextUnformatted(buf);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("View Matrix Viewer"))
        {
            if (ImGui::BeginTable("View", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        snprintf(buf, sizeof(buf), "%f", viewMatrix[row][column]);
                        ImGui::TextUnformatted(buf);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Model Matrix Viewer"))
        {
            if (ImGui::BeginTable("Model", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        snprintf(buf, sizeof(buf), "%f", modelMatrix[row][column]);
                        ImGui::TextUnformatted(buf);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        ImGui::SliderFloat("Translate X", &mikePosition.x, -displayWidth, displayWidth);
        ImGui::SliderFloat("Translate Y", &mikePosition.y, -displayWidth, displayWidth);
        ImGui::SliderFloat("Translate Z", &mikePosition.z, -8000.0f, 8000.0f);
        ImGui::SliderFloat("Rotate X", &mikeRotation.x, 0, 360);
        ImGui::SliderFloat("Rotate Y", &mikeRotation.y, 0, 360);
        ImGui::SliderFloat("Rotate Z", &mikeRotation.z, 0, 360);
        ImGui::SliderFloat("Scale X", &mikeScale.x, -2, 4);
        ImGui::SliderFloat("Scale Y", &mikeScale.y, -2, 4);
        ImGui::SliderFloat("Center X", &mikeCenterPoint.x, 0, 512);
        ImGui::SliderFloat("Center Y", &mikeCenterPoint.y, 0, 512);
        ImGui::SliderFloat("Center Z", &mikeCenterPoint.z, 0, 512);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
    }

};


int main(int argc, char *argv[])
{
    DemoApp app;
    return app.run("ProjectionTester", 800, 600);
}
