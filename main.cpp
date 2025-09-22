#if 0


#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <ctime>

#include "AudioFile.h"


#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "functions.h"
#include "Camera.h"
#include "constants.h"

constexpr double PI = 3.14159265358979323846;


int main() {
	std::cout << "hello world\n";
	GLFWwindow* window = OpenGlInitiate();
	if (!window)
	{
		return -1;
	}

	camera.bindCallbackToWindow(window);

	float quadW = 0.2f;
	float quadL = 1.2f;
	float quadVertices[] = {
		// positions					// colors           // texture coords
		  quadW / 2,  0, -quadL / 2,	0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
		  quadW / 2,  0, 0,				0.0f, 1.0f, 0.0f,	1.0f, 0.0f,   // bottom right
		 -quadW / 2,  0, 0,				0.0f, 1.0f, 0.0f,	0.0f, 0.0f,   // bottom left
		 -quadW / 2,  0, -quadL / 2,	0.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int quadIndices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	VertexBuffer* QuadVbPtr = new VertexBuffer(quadVertices, sizeof(quadVertices));
	QuadVbPtr->Push<float>(3, false);
	QuadVbPtr->Push<float>(3, false);
	QuadVbPtr->Push<float>(2, false);
	IndexBuffer* QuadIbPtr = new IndexBuffer(quadIndices, sizeof(quadIndices) / sizeof(quadIndices[0]));
	VertexArray* QuadVaPtr = new VertexArray(QuadVbPtr, QuadIbPtr);
	Shader* QuadShaderPtr = new Shader("shader/basic/basic.vert", "shader/basic/basic.frag");
	//Texture LeafTex("asset/container.jpg");
	//Texture LeafTex("asset/head.png");

	float barW = 0.1f;
	float barL = 0.6f;
	float BarVertices[] = {
		// positions					// normal           // texture coords
		  barW / 2,  barL / 2,	0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 1.0f,   // top right
		  barW / 2,  0,			0.0f, 0.0f, 0.0f, 1.0f,		1.0f, 0.0f,   // bottom right
		 -barW / 2,  0,			0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,   // bottom left
		 -barW / 2,  barL / 2,	0.0f, 0.0f, 0.0f, 1.0f,		0.0f, 1.0f    // top left 
	};
	unsigned int BarIndices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	VertexBuffer* BarVbPtr = new VertexBuffer(BarVertices, sizeof(BarVertices));
	BarVbPtr->Push<float>(3, false);
	BarVbPtr->Push<float>(3, false);
	BarVbPtr->Push<float>(2, false);
	IndexBuffer* BarIbPtr = new IndexBuffer(BarIndices, sizeof(BarIndices) / sizeof(BarIndices[0]));
	VertexArray* BarVaPtr = new VertexArray(BarVbPtr, BarIbPtr);
	Shader* BarShaderPtr = new Shader("shader/rect/rect.vert", "shader/rect/rect.frag");
	glm::vec3 BarColor = glm::vec3(1.0f);




	glm::mat4 Model(1.0f);
	glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	float MoveSpeed = 1;

	glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
	glm::vec3 LightColor = glm::vec3(1.0);
	glm::vec4 LightDirPos = glm::normalize(glm::vec4(0.0, 1.0f, 1.0f, 1.0f));

	float lastTime = glfwGetTime();
	//camera.SetPosition(glm::vec3(0, 1.0, 3.0));

	AudioFile<double> audioFile;
	audioFile.load("asset/wav/foam.wav");
	audioFile.printSummary();

	double DurationTimeInSeconds = audioFile.getLengthInSeconds();

	int channel = 0;
	int numSamples = audioFile.getNumSamplesPerChannel();

	for (int i = 0; i < 10; i++)
	{
		double currentSample = audioFile.samples[channel][i*10];
		std::cout << currentSample << " ";
	}
	double PlayTime = 0;
	unsigned int PlayIndex = 0;

	double RecordTime = 0.0;
	double TransTime = 0.1f; // 由当前 ScaleH 线性变换到下一个 ScaleH的时间
	double KeepTime = 0.05f; // 变到那个 ScaleH 后，位置一段时间
	double Period = 0.25;
	float ScaleH = 1.0f;
	float TargetScaleH = 1.0f;
	float LastScaleH = 1.0f;
	while (!glfwWindowShouldClose(window)) {

		float currentTime = glfwGetTime();
		float displacement = MoveSpeed * (currentTime - lastTime);

		if (RecordTime + Period > currentTime)
		{
			if (currentTime < RecordTime + TransTime)
			{
				float frac = (currentTime - RecordTime) / TransTime;
				ScaleH = frac * TargetScaleH + (1 - frac) * LastScaleH;
			}
			else if (currentTime < RecordTime + KeepTime) // 高度保持一段时间
			{
				ScaleH = ScaleH;
			}
			else {
				ScaleH *= 0.99f;
			}
		}
		else { // 高度变化
			PlayTime += (currentTime - RecordTime);
			if (PlayTime > DurationTimeInSeconds)
			{
				PlayTime = 0;
			}
			PlayIndex = (PlayTime / DurationTimeInSeconds) * numSamples;
			LastScaleH = TargetScaleH;
			TargetScaleH = std::abs(audioFile.samples[0][PlayIndex]);
			RecordTime = currentTime;
		}

		float r = std::abs(audioFile.samples[0][PlayIndex]);
		float g = std::abs(audioFile.samples[0][PlayIndex]);
		BarColor.x = r;
		BarColor.y = g;
		
		
		//std::cout << PlayIndex <<" mag: " << Scale << std::endl;
		lastTime = currentTime;

		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		// render here
		GLCall(glClearColor(0.9f, 0.9f, 0.9f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		camera.updateCamera(window);

		QuadShaderPtr->Use();
		QuadShaderPtr->setView(camera, window);

		Model = glm::translate(Model, MoveDirection * displacement);

		if (Model[3][2] - quadL> camera.getPosition().z) {
			Model = glm::mat4(1.0f);
		}

		QuadShaderPtr->SetUniformMat4(Model, "model");
		QuadShaderPtr->SetUniformMat4(TIModel, "TIModel");
		QuadShaderPtr->SetUniformV3(LightColor, "LightColor");
		QuadShaderPtr->SetUniformV4(LightDirPos, "LightDirPos");
		QuadShaderPtr->SetUniformV3(camera.getPosition(), "ViewPos");
		QuadVaPtr->DrawElement(*QuadShaderPtr);
		QuadShaderPtr->Unuse();

		BarShaderPtr->Use();
		BarShaderPtr->setView(camera, window);
		BarShaderPtr->SetUniformMat4(TIModel, "TIModel");
		BarShaderPtr->SetUniformV3(LightColor, "LightColor");
		BarShaderPtr->SetUniformV4(LightDirPos, "LightDirPos");
		BarShaderPtr->SetUniformV3(camera.getPosition(), "ViewPos");
		BarShaderPtr->SetUniform1f(ScaleH, "ScaleH");
		BarShaderPtr->SetUniformV3(BarColor, "ObjectColor");
		BarVaPtr->DrawElement(*BarShaderPtr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete(QuadVaPtr);

	glfwTerminate();
}


#endif