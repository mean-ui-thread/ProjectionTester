#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform2.hpp>

#include "AttributeInfo.h"
#include "ShaderProgram.h"
#include "BaseApp.h"
#include "Texture.h"
#include "VertexBuffer.h"

struct DemoVertex {
	glm::vec2 position;
	glm::u8vec4 color;
};


struct DemoApp : public BaseApp
{
	ShaderProgram *fillProgram = NULL;
	VertexBuffer *backgroundVBO = NULL;
	VertexBuffer *redRectVBO = NULL;
	VertexBuffer *greenRectVBO = NULL;
	VertexBuffer *blueRectVBO = NULL;
	bool useOrtho = false;
	bool useFrontToBack = true;
	float fieldOfView = 35.0f;
	glm::vec3 greenRotation = glm::vec3(45.0f);
	glm::vec2 vanishPoint = glm::vec2(256.0f+64.0f, 128.0f+64.0f);

	std::vector<DemoVertex> backgroundVertices = {
		//{   X       Y   }  { R     G     B     A  }
		{ {  0.0f,   0.0f}, {0xFF, 0xBF, 0x80, 0xFF} },
		{ {  0.0f, 480.0f}, {0xFF, 0xBF, 0x80, 0xFF} },
		{ {800.0f,   0.0f}, {0xFF, 0xBF, 0x80, 0xFF} },
		{ {800.0f, 480.0f}, {0xFF, 0xBF, 0x80, 0xFF} },
	};

	std::vector<DemoVertex> redRectVertices = {
		//{   X       Y   }  { R     G     B     A  }
		{ {   0.0f,   0.0f}, {0xFF, 0x00, 0x00, 0xFF} },
		{ {   0.0f, 128.0f}, {0xFF, 0x00, 0x00, 0xFF} },
		{ { 128.0f,   0.0f}, {0xFF, 0x00, 0x00, 0xFF} },
		{ { 128.0f, 128.0f}, {0xFF, 0x00, 0x00, 0xFF} },
	};

	std::vector<DemoVertex> greenRectVertices = {
		//{   X       Y   }  { R     G     B     A  }
		{ {   0.0f,   0.0f}, {0x00, 0xFF, 0x00, 0xFF} },
		{ {   0.0f, 128.0f}, {0x00, 0xFF, 0x00, 0xFF} },
		{ { 128.0f,   0.0f}, {0x00, 0xFF, 0x00, 0xFF} },
		{ { 128.0f, 128.0f}, {0x00, 0xFF, 0x00, 0xFF} },
	};

	std::vector<DemoVertex> blueRectVertices = {
		//{   X       Y   }  { R     G     B     A  }
		{ {   0.0f,   0.0f}, {0x00, 0x00, 0xFF, 0xFF} },
		{ {   0.0f, 128.0f}, {0x00, 0x00, 0xFF, 0xFF} },
		{ { 128.0f,   0.0f}, {0x00, 0x00, 0xFF, 0xFF} },
		{ { 128.0f, 128.0f}, {0x00, 0x00, 0xFF, 0xFF} },
	};

	glm::mat4 projectionMatrix;
	glm::mat4 modelMatrix;

	GLint u_MVP = 0;

	virtual bool init() override
	{
		Shader fillVert("fill.vert");
		if (fillVert.compile() != 0)
		{
			return false;
		}

		Shader fillFrag("fill.frag");
		if (fillFrag.compile() != 0)
		{
			return false;
		}

		std::vector<AttributeInfo> fillAttributes = {
			{"a_position", AttributeInfo::Float, 2, false},
			{"a_color", AttributeInfo::UnsignedByte, 4, true},
		};

		fillProgram = new ShaderProgram(fillAttributes);
		fillProgram->attach(&fillVert);
		fillProgram->attach(&fillFrag);
		if (fillProgram->link() != 0)
		{
			return false;
		}

		u_MVP      = fillProgram->getUniformLocation("u_MVP");

		backgroundVBO = new VertexBuffer();
		backgroundVBO->upload(backgroundVertices, VertexBuffer::Static);

		redRectVBO = new VertexBuffer();
		redRectVBO->upload(redRectVertices, VertexBuffer::Static);

		greenRectVBO = new VertexBuffer();
		greenRectVBO->upload(greenRectVertices, VertexBuffer::Static);

		blueRectVBO = new VertexBuffer();
		blueRectVBO->upload(blueRectVertices, VertexBuffer::Static);

		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDepthRange(0.0, 1.0);

		return true;
	}

	virtual void shutdown() override
	{
		delete fillProgram;
		fillProgram = NULL;

		delete backgroundVBO;
		backgroundVBO = NULL;

		delete redRectVBO;
		redRectVBO = NULL;

		delete greenRectVBO;
		greenRectVBO = NULL;

		delete blueRectVBO;
		blueRectVBO = NULL;
	}

	void drawBackground()
	{
		glm::mat4 proj = projectionMatrix;

		proj[3][2] = 1.0f;

		glm::mat4 mvp = proj;

		fillProgram->setUniform(u_MVP, mvp);
		backgroundVBO->bind(fillProgram);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)backgroundVertices.size());
	}

	void drawRedRect()
	{

		glm::mat4 proj = projectionMatrix;

		glm::mat4 modelMatrix  = glm::translate(glm::identity<glm::mat4>(), glm::vec3(128.0f, 128.0f, 0.0f));

		proj[3][2] = 0.75f;

		glm::mat4 mvp = proj * modelMatrix;


		fillProgram->setUniform(u_MVP, mvp);
		redRectVBO->bind(fillProgram);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)redRectVertices.size());
	}



	void drawGreenRect()
	{
		// set vanishing point
		if (!useOrtho) {
			float vpx = vanishPoint.x;
            float vpy = displayHeight - vanishPoint.y;
			projectionMatrix[2][0] = (2.0f * vpx /  displayWidth) - 1.0f;
			projectionMatrix[2][1] = (2.0f * vpy / displayHeight) - 1.0f;
		}

		glm::mat4 proj = projectionMatrix;

		glm::mat4 modelMatrix  = glm::translate(glm::identity<glm::mat4>(), glm::vec3(256.0f+64.0f, 128.0f+64.0f, 0.0f));
		modelMatrix *= glm::yawPitchRoll(glm::radians(0.0f), glm::radians(greenRotation.x), glm::radians(0.0f));
		modelMatrix  = glm::translate(modelMatrix, glm::vec3(-64.0f, -64.0f, -0.0f));

		proj[3][2] = 0.25f;

		glm::mat4 mvp = proj * modelMatrix;


		fillProgram->setUniform(u_MVP, mvp);
		greenRectVBO->bind(fillProgram);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)greenRectVertices.size());
	}

	void drawBlueRect()
	{
		glm::mat4 proj = projectionMatrix;
		glm::mat4 modelMatrix  = glm::translate(glm::identity<glm::mat4>(), glm::vec3(384.0f, 128.0f, 0.0f));

		proj[3][2] = 0.00;

		glm::mat4 mvp = proj * modelMatrix;


		fillProgram->setUniform(u_MVP, mvp);
		blueRectVBO->bind(fillProgram);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)blueRectVertices.size());
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
			projectionMatrix[2][2] = 0.0f;
		}

		fillProgram->bind();

		if (useFrontToBack) {
			glEnable(GL_DEPTH_TEST);
			drawBlueRect();
			drawGreenRect();
			drawRedRect();
			drawBackground();
		} else {
			glDisable(GL_DEPTH_TEST);
			drawBackground();
			drawRedRect();
			drawGreenRect();
			drawBlueRect();
		}
	}

	virtual void renderUI() override
	{
		char buf[32];

		ImGui::NewFrame();

		//		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		//		ImGui::SetNextWindowSize(ImVec2(displayWidth, displayHeight), ImGuiCond_Always);
		//		ImGui::Begin("VanishPointGizmo", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
		//		ImGui::GetWindowDrawList()->AddLine(ImVec2(vanishPoint.x, vanishPoint.y-16), ImVec2(vanishPoint.x, vanishPoint.y+16), IM_COL32(255, 0, 0, 255), 3.0f);
		//		ImGui::GetWindowDrawList()->AddLine(ImVec2(vanishPoint.x-16, vanishPoint.y), ImVec2(vanishPoint.x+16, vanishPoint.y), IM_COL32(255, 0, 0, 255), 3.0f);
		//		ImGui::End();

		ImGui::Begin("Control Panel");
		ImGui::Checkbox("Use Orthographic Projection", &useOrtho);
		ImGui::Checkbox("Front to Back", &useFrontToBack);
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

		//		ImGui::SliderFloat("Translate X", &mikePosition.x, -displayWidth, displayWidth);
		//		ImGui::SliderFloat("Translate Y", &mikePosition.y, -displayWidth, displayWidth);
		//		ImGui::SliderFloat("Translate Z", &mikePosition.z, -8000.0f, 8000.0f);
				ImGui::SliderFloat("Rotate X", &greenRotation.x, 0, 360);
		//		ImGui::SliderFloat("Rotate Y", &mikeRotation.y, 0, 360);
		//		ImGui::SliderFloat("Rotate Z", &mikeRotation.z, 0, 360);
		//		ImGui::SliderFloat("Scale X", &mikeScale.x, -2, 4);
		//		ImGui::SliderFloat("Scale Y", &mikeScale.y, -2, 4);
		//		ImGui::SliderFloat("Center X", &mikeCenterPoint.x, 0, 512);
		//		ImGui::SliderFloat("Center Y", &mikeCenterPoint.y, 0, 512);
		//		ImGui::SliderFloat("Center Z", &mikeCenterPoint.z, 0, 512);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
	}

};


int main(int argc, char *argv[])
{
	DemoApp app;
	return app.run("ProjectionTester", 800, 480);
}
