#if 0


#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OsmXmlData.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include "Camera.h"
#include "constants.h"

#include "functions.h"

constexpr double M_PI = 3.14159265358979323846;



int main()
{

    GLFWwindow* window = OpenGlInitiate();
    if (!window)
    {
        return -1;
    }

    camera.bindCallbackToWindow(window);

    std::string XmlFile = "map/map.osm";
    Osm::OsmXmlData OsmData;
    OsmData.ReadXmlFile(XmlFile.c_str());

    std::cout << "node size: " << OsmData.Nodes.size() << std::endl;
    std::cout << "way size: " << OsmData.Ways.size() << std::endl;

    std::vector<Osm::VertexAttributes> Vertices;
    double lat0 = 31.2;   // 参考点纬度
    double lon0 = 121.392;  // 参考点经度
    double R = 100000.0f; // 地球半径，米
    // 把各个顶点的经纬度转变为横纵坐标
    for (auto& node : OsmData.Nodes)
    {

        double x = (node.Longitude - lon0) * M_PI / 180.0 * R * cos(lat0 * M_PI / 180.0);
        double z = (node.Latitude - lat0) * M_PI / 180.0 * R;
        glm::vec3 Pos(x, 0, z);
        glm::vec3 Norm(0, 0, 1);
        Vertices.emplace_back(Pos, Norm);

    }

    std::vector<Osm::VertexAttributes> BuildingVertices;
    std::vector<unsigned int> BuildingIndices;
    std::vector<Osm::VertexAttributes> BuildingSideFaceVertices; // 绘制建筑侧面所需要的顶点，不使用下标索引

    std::vector<unsigned int> Indices;
    int wayCnt = 0;
    int buildingCnt = 0;

    // 地铁
    std::vector<Osm::VertexAttributes> RaiLwayVertices;
    std::vector<unsigned int> RailwayIndices;

    for (auto& way : OsmData.Ways)
    {

        bool IsBuilding = way.Tags.count("building");
        //bool IsRailWay = way.Tags.count("railway") && way.Tags.count("name:en");
        //if (IsRailWay) {
        //    std::cout << way.Tags["name:en"] << std::endl;
        //}
        std::vector<Osm::VertexAttributes> BottomBuildingVertices;
        for (int i = 1; i < way.NodeIds.size(); ++i)
        {
            unsigned int Index0 = OsmData.NodeIdMap[way.NodeIds[i - 1]];
            unsigned int Index1 = OsmData.NodeIdMap[way.NodeIds[i]];
            Indices.push_back(Index0);
            Indices.push_back(Index1);
            //std::cout << Index0 << "->" << Index1 << std::endl;
            /*std::cout << OsmData.Nodes[Index0].Latitude << " " << OsmData.Nodes[Index0].Longitude << ": ";
            std::cout << Vertices[Index0].Position.x <<" " << Vertices[Index0].Position.y <<" " << Vertices[Index0].Position.z << std::endl;*/

            // 如果当前是建筑的信息，保存下来，后面生成三角形面片
            if (IsBuilding )
            {

                if (1 == i) {
                    BottomBuildingVertices.emplace_back(Vertices[Index0]);
                }
                BottomBuildingVertices.emplace_back(Vertices[Index1]);
            }
            //if (IsRailWay)
            //{
            //    if (1 == i)
            //    {
            //        RaiLwayVertices.emplace_back(Vertices[Index0]);
            //    }
            //    RailwayIndices.emplace_back(Vertices[Index1]);
            //}
        }

        if (BottomBuildingVertices.size() && IsBuilding)
        {
            // 绘制底部
            buildingCnt++;
            if (BottomBuildingVertices[0].Position == BottomBuildingVertices.back().Position)
            {
                BottomBuildingVertices.pop_back();
            }

            std::vector<unsigned int> CurBuildingIndices = Osm::EarClip(BottomBuildingVertices, BuildingVertices.size());
            BuildingVertices.insert(BuildingVertices.end(), BottomBuildingVertices.begin(), BottomBuildingVertices.end());
            BuildingIndices.insert(BuildingIndices.end(), CurBuildingIndices.begin(), CurBuildingIndices.end());
            
            
            // 绘制顶部
            float Height = 0.5f;
            std::vector<Osm::VertexAttributes> TopBuildingVertices;
            for (auto& VA : BottomBuildingVertices)
            {
                glm::vec3 Pos = glm::vec3(VA.Position.x, VA.Position.y + Height, VA.Position.z);
                TopBuildingVertices.emplace_back(Pos, glm::vec3(0, 1, 0));
            }
            std::vector<unsigned int> TopBuildingIndices = Osm::EarClip(TopBuildingVertices, BuildingVertices.size());
            BuildingVertices.insert(BuildingVertices.end(), TopBuildingVertices.begin(), TopBuildingVertices.end());
            BuildingIndices.insert(BuildingIndices.end(), TopBuildingIndices.begin(), TopBuildingIndices.end());

            // 侧面的信息
            std::vector<Osm::VertexAttributes> SideVertices = Osm::GenerateSideFaces(BottomBuildingVertices, TopBuildingVertices);
            BuildingSideFaceVertices.insert(BuildingSideFaceVertices.end(), SideVertices.begin(), SideVertices.end());
        }
        wayCnt++;
    }

    std::cout << "building vertices number: " << BuildingVertices.size() << std::endl;

    // 绘制建筑
    VertexBuffer* BuildingVbPtr = new VertexBuffer(BuildingVertices.data(), BuildingVertices.size() * sizeof(BuildingVertices[0]));
    BuildingVbPtr->Push<float>(3, false);
    BuildingVbPtr->Push<float>(3, false);
    IndexBuffer*  BuildingIbPtr = new IndexBuffer(BuildingIndices.data(), BuildingIndices.size());
    VertexArray* BuildingVaPtr = new VertexArray(BuildingVbPtr, BuildingIbPtr);
    Shader* BuildingShader = new Shader("shader/building/building.vert", "shader/building/building.frag");
    glm::vec3 BuildingColor = glm::vec3(0.92f, 0.8966f, 0.8837f);
    // 绘制建筑的顶点法线
    Shader* NormalShaderPtr = new Shader("shader/geom/geom.vert", "shader/geom/geom.frag", "shader/geom/geom.geom");

    VertexBuffer* BuildingSideFacesVbPtr = new VertexBuffer(BuildingSideFaceVertices.data(), BuildingSideFaceVertices.size() * sizeof(BuildingSideFaceVertices[0]));
    BuildingSideFacesVbPtr->Push<float>(3, false);
    BuildingSideFacesVbPtr->Push<float>(3, false);
    VertexArray* BuildingSideFacesVaPtr = new VertexArray(BuildingSideFacesVbPtr);

    // 以线条的方式绘制所有的点
    VertexBuffer* AllVerticesVbPtr = new VertexBuffer(Vertices.data(), Vertices.size() * sizeof(Vertices[0]));
    AllVerticesVbPtr->Push<float>(3, false);
    AllVerticesVbPtr->Push<float>(3, false);
    //BarVbPtr->Push<float>(2, false);
    IndexBuffer* BarIbPtr = new IndexBuffer(Indices.data(), Indices.size());
    VertexArray* BarVaPtr = new VertexArray(AllVerticesVbPtr, BarIbPtr);
    BarIbPtr->Bind();
    Shader* BarShaderPtr = new Shader("shader/map/map.vert", "shader/map/map.frag");
    glm::vec3 BarColor = glm::vec3(0.5f,0.366f, 0.87f);


    // 绘制地铁
    Shader* RailwayShader = new Shader("shader/railway/railway.vert", "shader/railway/railway.frag", "shader/railway/railway.geom");
    Shader* tesellShader = new Shader("shader/basic/basic.vert", "shader/basic/basic.frag", nullptr, "shader/basic/basic.tcs.glsl", "shader/basic/basic.tes.glsl");



	glm::mat4 Model(1.0f);
	glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	float MoveSpeed = 1;

	glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
	glm::vec3 LightColor = glm::vec3(1.0);
	glm::vec4 LightDirPos = glm::normalize(glm::vec4(-0.33, 1.623f, 1.21f, 0.0f));

	float lastTime = glfwGetTime();
	//camera.SetPosition(glm::vec3(0, 1.0, 3.0));
    float ScaleH = 1.0f;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
		BarShaderPtr->SetUniform1f(ScaleH, "ScaleH");
		BarShaderPtr->SetUniformV3(BarColor, "ObjectColor");
        BarVaPtr->Bind();
        BarIbPtr->Bind();
        GLCall(glDrawElements(GL_LINES, BarIbPtr->GetCount(), GL_UNSIGNED_INT, nullptr));


        // 绘制建筑的顶部和底部
        BuildingShader->Use();
        BuildingShader->setView(camera, window);
        BuildingShader->SetUniformMat4(TIModel, "TIModel");
        BuildingShader->SetUniformV3(LightColor, "LightColor");
        BuildingShader->SetUniformV4(LightDirPos, "LightDirPos");
        BuildingShader->SetUniformV3(camera.getPosition(), "ViewPos");
        BuildingShader->SetUniform1f(ScaleH, "ScaleH");
        BuildingShader->SetUniformV3(BuildingColor, "ObjectColor");
        BuildingVaPtr->Bind();
        BuildingIbPtr->Bind();
        BuildingVaPtr->DrawElement(*BuildingShader);


        // 绘制建筑的侧面

        BuildingSideFacesVaPtr->Bind();
        BuildingSideFacesVaPtr->DrawArray(*BuildingShader);

        // 绘制法线
        NormalShaderPtr->Use();
        NormalShaderPtr->setView(camera, window);
        NormalShaderPtr->SetUniformMat4(TIModel, "TIModel");
        NormalShaderPtr->SetUniform1f(ScaleH, "ScaleH");
        BuildingSideFacesVaPtr->Bind();
        BuildingSideFacesVaPtr->DrawArray(*NormalShaderPtr);

        BuildingVaPtr->Bind();
        BuildingIbPtr->Bind();
        BuildingVaPtr->DrawElement(*NormalShaderPtr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    delete(BuildingShader);
    delete(BarShaderPtr);

	glfwTerminate();
}

#endif