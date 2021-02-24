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
    bool useOrtho = false;
    bool useFrontToBack = true;
    float fieldOfView = 45.0f;
    glm::vec2 vanishPoint = glm::vec3(0.0f);
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

        // Vanish point initially the center of the screen
        vanishPoint.x = displayWidth * 0.5f;
        vanishPoint.y = displayHeight * 0.5f;

        // center initially at the center of mike
        mikeCenterPoint.x = 256.0f;
        mikeCenterPoint.y = 256.0f;

        // mike initially at the top left corer of the screen
        mikePosition.x = 256.0f;
        mikePosition.y = 256.0f;

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

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

    void drawMike()
    {
        // Draw mike
        // set vanishing point
        if (!useOrtho) {
            float vpx = vanishPoint.x;
            float vpy = displayHeight - vanishPoint.y;
            projectionMatrix[2][0] = (2.0f * vpx /  displayWidth) - 1.0f;
            projectionMatrix[2][1] = (2.0f * vpy / displayHeight) - 1.0f;
        }
        defaultProgram->setUniform(u_MVP, projectionMatrix * modelMatrix);
        mikeVBO->bind(defaultProgram);
        mikeTex->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)mikeVertices.size());
    }

    void drawBackground()
    {
        // Draw the background
        defaultProgram->setUniform(u_MVP, projectionMatrix); // No Model transforms for the background.
        backgroundVBO->bind(defaultProgram);
        backgroundTex->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)backgroundVertices.size());
    }


    virtual void render() override
    {
        if (useOrtho)
        {
            projectionMatrix = glm::ortho(0.0f, (float)displayWidth, (float)displayHeight, 0.0f, -8000.0f, 8000.0f);
        }
        else
        {
            float fieldOfViewRad = glm::radians(fieldOfView);
            float cameraDistance = ((float)displayHeight/2.0f) / tan(fieldOfViewRad/2.0f);
            float aspectRatio = (float)displayWidth / (float)displayHeight;

            projectionMatrix = glm::perspective(fieldOfViewRad, aspectRatio, 0.1f, cameraDistance+512.0f);
            projectionMatrix = glm::rotate(projectionMatrix, glm::radians(180.0f), glm::vec3(1, 0, 0));
            projectionMatrix = glm::translate(projectionMatrix, glm::vec3(-displayWidth*0.5f, -displayHeight*0.5f, cameraDistance));
        }

        // Order matters. We use TRSC (Translate, Rotate, Scale, Center)
        modelMatrix  = glm::translate(glm::identity<glm::mat4>(), mikePosition);
        modelMatrix *= glm::yawPitchRoll(glm::radians(mikeRotation.y), glm::radians(mikeRotation.x), glm::radians(mikeRotation.z));
        modelMatrix  = glm::scale(modelMatrix, mikeScale);
        modelMatrix  = glm::translate(modelMatrix, -mikeCenterPoint);

        defaultProgram->bind();
        defaultProgram->setUniform(u_texture0, 0);

        if (useFrontToBack) {
            glEnable(GL_DEPTH_TEST);
            projectionMatrix[2][2] = 0.0f;

            projectionMatrix[3][2] = 0.0f;
            drawMike();

            projectionMatrix[3][2] = 0.1f;
            drawBackground();
        } else {
            glDisable(GL_DEPTH_TEST);
            drawBackground();
            drawMike();
        }
    }

    virtual void renderUI() override
    {
        char buf[32];

        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(displayWidth, displayHeight), ImGuiCond_Always);
        ImGui::Begin("VanishPointGizmo", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(vanishPoint.x, vanishPoint.y-16), ImVec2(vanishPoint.x, vanishPoint.y+16), IM_COL32(255, 0, 0, 255), 3.0f);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(vanishPoint.x-16, vanishPoint.y), ImVec2(vanishPoint.x+16, vanishPoint.y), IM_COL32(255, 0, 0, 255), 3.0f);
        ImGui::End();

        ImGui::Begin("Control Panel");
        ImGui::Checkbox("Use Orthographic Projection", &useOrtho);
        ImGui::Checkbox("Front to Back", &useFrontToBack);
        ImGui::SliderFloat("Field of View", &fieldOfView, 0, 180);
        ImGui::SliderFloat("Vanish Point X", &vanishPoint.x, 0, displayWidth);
        ImGui::SliderFloat("Vanish Point Y", &vanishPoint.y, 0, displayHeight);

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
