#if 1


#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
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

struct ColorRgb
{
    UINT8 Red;
    UINT8 Green;
    UINT8 Blue;
    ColorRgb(float value){
        UINT8 v = static_cast<UINT8>(std::clamp(value * 255.0f, 0.0f, 255.0f));
        Red = v;
        Green = v;
        Blue = v;
    }
};

//std::vector<glm::u8vec3> WorleyNoise2D(std::vector<int> Dimension, int CellNumber)
//{
//    float xDim = Dimension[0];
//    float yDim = Dimension[1];
//
//}

std::vector<ColorRgb> WorleyNoise3D(std::vector<int>& Dimension, int CellNumber)
{

    
    float xDim = Dimension[0];
    float yDim = Dimension[1];
    float zDim = Dimension[2];

    int Size = xDim * yDim * zDim;
    std::vector<float> data;
    data.reserve(Size);
    std::vector<glm::vec3> FeaturePoints;
    for (int i = 0; i < CellNumber * CellNumber * CellNumber; ++i)
    {
        float RandX = GenRandom(0.0, 1.0);
        float RandY = GenRandom(0.0, 1.0);
        float RandZ = GenRandom(0.0, 1.0);
        FeaturePoints.emplace_back(RandX, RandY, RandZ);
    }

    float GlobalMin = 1e9; 
    float GlobalMax = 0; 
    for (int x = 0; x < xDim; ++x)
    {
        for (int y = 0; y < yDim; ++y)
        {
            for (int z = 0; z < zDim; ++z)
            {
                float gx = x / xDim * CellNumber;
                float gy = y / xDim * CellNumber;
                float gz = z / xDim * CellNumber;
                int cx = int(gx);
                int cy = int(gy);
                int cz = int(gz);

                float MinDist = 1e9;

                for (int dz = cz - 1; dz < cz + 2; ++dz)
                {
                    for (int dy = cy - 1; dy < cy + 2; ++dy)
                    {
                        for (int dx = cx - 1; dx < cx + 2; ++dx)
                        {
                            if (0 <= dx && dx < CellNumber && 0 <= dy && dy < CellNumber && 0 <= dz && dz < CellNumber)
                            {
                                int index = dx * (CellNumber * CellNumber) + dy * CellNumber + dz;
                                glm::vec3 fp = FeaturePoints[index];
                                glm::vec3 diff= glm::vec3(gx, gy, gz) - (glm::vec3(dx, dy, dz) + fp);
                                float distance = glm::length(diff);
                                if (distance < MinDist)
                                {
                                    MinDist = distance;
                                }
                            }
                        }
                    }
                }
                data.emplace_back(MinDist);
                GlobalMin = min(MinDist, GlobalMin);
                GlobalMax = max(MinDist, GlobalMax);
            }
        }
    }
    float Range = GlobalMax - GlobalMin;
    std::vector<ColorRgb> Result;
    for (auto& value : data)
    {
        float red =(value - GlobalMin) / Range;
        Result.emplace_back(red);
    }
    return Result;
}

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

    std::vector<float> Vertices = {
        -0.5, -0.5, 0, 0, 0,
         0.5, -0.5, 0, 1, 0,
         0.5,  0.5, 0, 1, 1,
        -0.5,  0.5, 0, 0, 1,
    };

    std::vector<unsigned int> Indices = {
        0, 1, 2,
        0, 2, 3,
    };



    // 以线条的方式绘制所有的点
    VertexBuffer* AllVerticesVbPtr = new VertexBuffer(Vertices.data(), Vertices.size() * sizeof(Vertices[0]));
    AllVerticesVbPtr->Push<float>(3, false);
    AllVerticesVbPtr->Push<float>(2, false);

    //BarVbPtr->Push<float>(2, false);
    IndexBuffer* IbPtr = new IndexBuffer(Indices.data(), Indices.size());
    VertexArray* VaPtr = new VertexArray(AllVerticesVbPtr, IbPtr);
    IbPtr->Bind();
    Shader* ShaderPtr = new Shader("shader/volumetricCloud/volumetricCloud.vert", "shader/volumetricCloud/volumetricCloud.frag");
    

    std::vector<int> dimension = { 128, 128, 128 };
    std::vector<ColorRgb> data = WorleyNoise3D(dimension, 8);

    std::cout << "date 0: " << (int)data[0].Red << " " << (int)data[0].Green << " " << (int)data[0].Blue << std::endl;

    // 生成一个 3D 纹理
    GLuint tex3DId{ 0 };
    glGenTextures(1, &tex3DId);
    glBindTexture(GL_TEXTURE_3D, tex3DId);

    // 环绕方式（XYZ 三个方向）
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    // 过滤方式
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 传数据到 GPU
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 128, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());


    glm::mat4 Model(1.0f);
    glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    float MoveSpeed = 1;

    glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
    glm::vec3 LightColor = glm::vec3(1.0);
    glm::vec4 LightDirPos = glm::normalize(glm::vec4(-0.33, 1.623f, 1.21f, 0.0f));

    float lastTime = glfwGetTime();


    while (!glfwWindowShouldClose(window)) {

        float currentTime = glfwGetTime();
        float displacement = MoveSpeed * (currentTime - lastTime);


        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);
        // render here
        GLCall(glClearColor(0.9f, 0.9f, 0.9f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        camera.updateCamera(window);



        // 会制 heighway
        ShaderPtr->Use();
        ShaderPtr->setView(camera, window);
        ShaderPtr->SetUniformMat4(TIModel, "TIModel");
        ShaderPtr->SetUniformV3(camera.getPosition(), "ViewPos");
        ShaderPtr->SetTexture(tex3DId, GL_TEXTURE0, "WorleyNoise", GL_TEXTURE_3D);
        VaPtr->Bind();
        IbPtr->Bind();
        VaPtr->DrawElement(*ShaderPtr);
        //GLCall(glDrawElements(GL_LINES, IbPtr->GetCount(), GL_UNSIGNED_INT, nullptr));


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete(ShaderPtr);

    glfwTerminate();
}

#endif