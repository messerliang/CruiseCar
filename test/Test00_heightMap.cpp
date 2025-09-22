#if 0

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OsmXmlData.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "constants.h"
#include "IndexBuffer.h"
#include <VertexBuffer.h>
#include <VertexArray.h>
#include "functions.h"

#include <opencv2/opencv.hpp>
#include <iostream>




int main()
{
	GLFWwindow* window = OpenGlInitiate();
	if (!window)
	{
		std::cout << "opengl initiate failed\n";
		return -1;
	}

	camera.bindCallbackToWindow(window);

	// ¶ÁÈ¡Í¼Ïñ£¨²ÊÉ«£¬BGR£©
	cv::Mat img = cv::imread("asset/img/iceland_heightmap.png", cv::IMREAD_GRAYSCALE);

	if (img.empty()) {
		std::cerr << "image read failed£¡" << std::endl;
		return -1;
	}

	std::cout << img.channels() << " " << img.rows << " " << img.cols << std::endl;

	int channels = img.channels();
	int width = img.cols;
	int height = img.rows;

	
	// vertex generation
	std::vector<float> vertices
	//	= {
	//	0,0,0,0,0,
	//	1,0,0,0,0,
	//	0.5,1,0,0,0,
	//}
	;

	unsigned rez = 20;
	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
		}
	}
	VertexBuffer* VbPtr = new VertexBuffer(vertices.data(), vertices.size() * sizeof(vertices[0]));
	VbPtr->Push<float>(3, false);
	VbPtr->Push<float>(2, false);
	Texture* heightMapTex = new Texture("asset/img/iceland_heightmap.png");
	VertexArray* VaPtr = new VertexArray(VbPtr);
	Shader* ShaderPtr = new Shader("shader/basic/basic.vert", "shader/basic/basic.frag", nullptr, "shader/basic/basic.tcs.glsl", "shader/basic/basic.tes.glsl");
	//Shader* ShaderPtr = new Shader("shader/basic/basic.vert", "shader/basic/basic.frag");


	glm::mat4 Model(1.0f);
	glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	float MoveSpeed = 1;

	glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
	glm::vec3 LightColor = glm::vec3(1.0);
	glm::vec4 LightDirPos = glm::normalize(glm::vec4(2.0, 1.0f, 1.0f, 0.0f));

	float lastTime = glfwGetTime();

	float ScaleH = 1.0f;

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {

		float currentTime = glfwGetTime();
		float displacement = MoveSpeed * (currentTime - lastTime);


		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		// render here
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		camera.updateCamera(window);



		ShaderPtr->Use();
		ShaderPtr->setView(camera, window);
		ShaderPtr->SetUniformMat4(TIModel, "TIModel");
		ShaderPtr->SetUniformV3(LightColor, "LightColor");
		ShaderPtr->SetUniformV4(LightDirPos, "LightDirPos");
		ShaderPtr->SetUniformV3(camera.getPosition(), "ViewPosition");
		ShaderPtr->SetTexture(*heightMapTex, GL_TEXTURE0, "heightMap");
	
		
		VaPtr->Bind(); 
		glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
		//VaPtr->DrawArray(*ShaderPtr);

		//VaPtr->DrawElement(*ShaderPtr);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
}

#endif