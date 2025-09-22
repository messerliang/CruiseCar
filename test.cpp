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

int main()
{
	GLFWwindow* window = OpenGlInitiate();
	if (!window)
	{
		std::cout << "opengl initiate failed\n";
		return -1;
	}

	camera.bindCallbackToWindow(window);

	std::vector<std::vector<glm::vec3>> PositionArr =
	{
		{
			glm::vec3(32.5725,- 37.2696, 0),
			glm::vec3(32.7775,-37.2097, 0),
			glm::vec3(32.7091,-36.9733, 0),
			glm::vec3(32.8116,-36.9434, 0),
			glm::vec3(32.7661,-36.7902, 0),
			glm::vec3(33.0508,-36.7037, 0),
			glm::vec3(33.2559,-37.4028, 0),
			glm::vec3(32.4927,-37.6325, 0),
			glm::vec3(32.4813,-37.5859, 0),
			glm::vec3(32.4813,-37.5892, 0),
			glm::vec3(32.5839,-37.2696, 0),

		},
		

	};



	std::vector<Osm::VertexAttributes> AllVertices;
	std::vector<unsigned int> AllIndices;
	
	std::vector<Osm::VertexAttributes> Vertices0;

	glm::vec3 Norm = glm::vec3(0, 0, 1);

	for (auto& Position : PositionArr)
	{
		std::vector<Osm::VertexAttributes> Vertices;
		for (auto& pos : Position)
		{
			Vertices.emplace_back(pos, Norm);
		}

		std::vector<unsigned int> Indices = Osm::EarClip(Vertices, AllVertices.size());
		AllVertices.insert(AllVertices.end(), Vertices.begin(), Vertices.end());;
		AllIndices.insert(AllIndices.end(), Indices.begin(), Indices.end());

	}
	glm::vec3 test(0, 0, -1);
	test = glm::normalize(test);
	std::cout << test.x << " " << test.y << " " << test.z << std::endl;


	std::cout << "finished" << std::endl;

	VertexBuffer* BarVbPtr = new VertexBuffer(AllVertices.data(), AllVertices.size() * sizeof(AllVertices[0]));
	BarVbPtr->Push<float>(3, false);
	BarVbPtr->Push<float>(3, false);
	//BarVbPtr->Push<float>(2, false);
	IndexBuffer* BarIbPtr = new IndexBuffer(AllIndices.data(), AllIndices.size());
	VertexArray* BarVaPtr = new VertexArray(BarVbPtr, BarIbPtr);
	BarIbPtr->Bind();
	Shader* BarShaderPtr = new Shader("shader/basic/basic.vert", "shader/basic/basic.frag");
	glm::vec3 BarColor = glm::vec3(0.5f, 0.366f, 0.87f);


	std::vector<glm::vec3> Positions = {
		glm::vec3(-0.1, 0, 0.0),
		glm::vec3(-0.1, 0, 0.1),
		glm::vec3(-0.2, 0, 0.1),
		glm::vec3(-0.2, 0, 0.0),

	};
	std::vector<Osm::VertexAttributes> ButtomVertices;
	std::vector<Osm::VertexAttributes> TopVertices;
	for (auto& Pos : Positions)
	{
		ButtomVertices.emplace_back(Pos, Norm);
		TopVertices.emplace_back(Pos + glm::vec3(0, 0.1, 0), Norm);
	}
	std::vector<Osm::VertexAttributes> SideVertices = Osm::GenerateSideFaces(ButtomVertices, TopVertices);
	VertexBuffer* SideVbPtr = new VertexBuffer(SideVertices.data(), SideVertices.size() * sizeof(SideVertices[0]));
	SideVbPtr->Push<float>(3, false);
	SideVbPtr->Push<float>(3, false);
	VertexArray* SideVaPtr = new VertexArray(SideVbPtr);


	glm::mat4 Model(1.0f);
	glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	float MoveSpeed = 1;

	glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
	glm::vec3 LightColor = glm::vec3(1.0);
	glm::vec4 LightDirPos = glm::normalize(glm::vec4(2.0, 1.0f, 1.0f, 0.0f));

	float lastTime = glfwGetTime();

	float ScaleH = 1.0f;

	while (!glfwWindowShouldClose(window)) {

		float currentTime = glfwGetTime();
		float displacement = MoveSpeed * (currentTime - lastTime);


		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		// render here
		GLCall(glClearColor(0.9f, 0.9f, 0.9f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		camera.updateCamera(window);



		BarShaderPtr->Use();
		BarShaderPtr->setView(camera, window);
		BarShaderPtr->SetUniformMat4(TIModel, "TIModel");
		BarShaderPtr->SetUniformV3(LightColor, "LightColor");
		BarShaderPtr->SetUniformV4(LightDirPos, "LightDirPos");
		BarShaderPtr->SetUniformV3(camera.getPosition(), "ViewPos");


		BarVaPtr->DrawElement(*BarShaderPtr);

		SideVaPtr->Bind();
		SideVaPtr->DrawArray(*BarShaderPtr);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
}

#endif