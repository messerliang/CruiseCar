#if 1

#include <omp.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <execution>

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
#include "SimplexNoise.h"

constexpr double M_PI = 3.14159265358979323846;


glm::vec3 hash3(glm::vec3 p)
{
    // 一种常见的哈希算法（基于 IQ 的实现）
    p = glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 74.7)),
        glm::dot(p, glm::vec3(269.5, 183.3, 246.1)),
        glm::dot(p, glm::vec3(113.5, 271.9, 124.6)));

    return glm::fract(sin(p) * 43758.5453123f);
}


float WorleyNoiseAtPosition(glm::vec3 position, int CellNumber, int seed=0)
{



    float gx = position.x;
    float gy = position.y;
    float gz = position.z;

    // 向下取整，所在的网格位置
    int cx = int(position.x);
    int cy = int(position.y);
    int cz = int(position.z);

    float MinDist = 1e9;

    for (int dz = cz - 1; dz < cz + 2; ++dz)
    {
        for (int dy = cy - 1; dy < cy + 2; ++dy)
        {
            for (int dx = cx - 1; dx < cx + 2; ++dx)
            {
                int mdx = (dx + CellNumber) % CellNumber;
                int mdy = (dy + CellNumber) % CellNumber;
                int mdz = (dz + CellNumber) % CellNumber;


                //if (0 <= dx && dx < CellNumber && 0 <= dy && dy < CellNumber && 0 <= dz && dz < CellNumber)
                //{

                    glm::vec3 fp = hash3(glm::vec3(mdx,mdy,mdz) + (float)seed);
                    glm::vec3 diff = glm::vec3(gx, gy, gz) - (glm::vec3(dx, dy, dz) + fp);
                    float distance = glm::length(diff);
                    if (distance < MinDist)
                    {
                        MinDist = distance;
                    }
                //}
            }
        }
    }

    return (1 - MinDist) / 1.7320508f;
}

std::vector<glm::vec3> WorleyNoiseF3D(std::vector<int>& Dimension, int CellNumber, int octaves=3, float freq=1.0f, float lacunarity = 2.0f, float gain=0.5)
{

    
    float xDim = Dimension[0];
    float yDim = Dimension[1];
    float zDim = Dimension[2];

    int Size = xDim * yDim * zDim;
    std::vector<glm::vec3> data;
    data.reserve(Size);
    //std::vector<glm::vec3> FeaturePoints;
    //for (int i = 0; i < CellNumber * CellNumber * CellNumber; ++i)
    //{
    //    float RandX = GenRandom(0.0, 1.0);
    //    float RandY = GenRandom(0.0, 1.0);
    //    float RandZ = GenRandom(0.0, 1.0);
    //    FeaturePoints.emplace_back(RandX, RandY, RandZ);
    //}

    float GlobalMin = 1e9; 
    float GlobalMax = 0; 
#pragma omp parallel for
    for (int x = 0; x < xDim; ++x)
    {
        std::cout << x << " ";
#pragma omp parallel for
        for (int y = 0; y < yDim; ++y)
        {
#pragma omp parallel for
            for (int z = 0; z < zDim; ++z)
            {

                //std::cout << z << " ";
                // 找出来在哪个网格
                float gx = x / xDim * CellNumber;
                float gy = y / xDim * CellNumber;
                float gz = z / xDim * CellNumber;
                glm::vec3 position(gx, gy, gz);
                float val = 0;
                float amplitude = 0.5f;
                float baseFreq = freq;
                for (int oc = 0; oc < octaves; ++oc)
                {
                    float n = WorleyNoiseAtPosition(position * baseFreq, CellNumber * baseFreq, oc);
                    val += amplitude * n;
                    amplitude *= gain;
                    baseFreq *= lacunarity;
                    
                }
                data.emplace_back(val);

            }
        }
    }

    return data;
}

GLuint GenTexture3D(void* data, int XDim, int YDim, int ZDim, int rgbFormat, int dataType)
{
    // 使用 rgbNoise 来生成 3D texture
    GLuint tex3D;
    glGenTextures(1, &tex3D);
    glBindTexture(GL_TEXTURE_3D, tex3D);

    // 假设你有一块 float 数组 data[w*h*d*4] (RGBA 格式)
    glTexImage3D(
        GL_TEXTURE_3D,     // target
        0,                 // mip level
        rgbFormat,          // internal format (GPU存储)
        XDim, YDim, ZDim,
        0,                 // border
        rgbFormat,           // data format
        dataType,           // data type
        data               // pointer
    );

    // 设置采样参数
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    return tex3D;
}

std::vector<glm::vec3> ReadNoise(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    size_t size = file.tellg() / sizeof(glm::vec3);
    file.seekg(0, std::ios::beg);
    std::cout << size << std::endl;
    std::vector<glm::vec3>rgbNoise(size);
    file.read(reinterpret_cast<char*>(rgbNoise.data()), size * sizeof(glm::vec3));
    return rgbNoise;
}

int main()
{
    //std::vector<ColorRgb>

    std::vector<glm::vec3> rgbNoise, perlinNoise, worleyNoise;
    int noiseType = 0;
    if (1 == noiseType) {
        SimplexNoise simplexNoise;
        FbmPerlin3D perlin3D;
        int XDim = 128;
        int YDim = 128;
        int ZDim = 128;
        for (float x = 0; x < XDim; x++)
        {
            for (float y = 0; y < YDim; y++)
            {
                for(float z = 0; z < ZDim; z++)
                {
                    float scale = 0.5;
                    float val = simplexNoise.fractal(8, scale*x, scale*y, scale*z);
                    rgbNoise.emplace_back((val + 1) / 2.);
                }
            }
        }
        const char* path = "asset/noiseTextures/fbmPerlin3D/data.bin";
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "无法打开文件\n";
            return 1;
        }

        file.write(reinterpret_cast<const char*>(rgbNoise.data()), rgbNoise.size() * sizeof(glm::vec3));
        file.close();

        const char* worleyPath = "asset/noiseTextures/fbmWorley3D/data.bin";
        worleyNoise = ReadNoise(worleyPath);

    }
    else if (2 == noiseType) {
        std::vector<int> dims = { 128,128,128 };

        worleyNoise = WorleyNoiseF3D(dims, 8, 5);
        const char* path = "asset/noiseTextures/fbmWorley3D/data.bin";
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "无法打开文件\n";
            return 1;
        }        
        file.write(reinterpret_cast<const char*>(rgbNoise.data()), rgbNoise.size() * sizeof(glm::vec3));
        file.close();

        const char* perlinPath = "asset/noiseTextures/fbmPerlin3D/data.bin";
        perlinNoise = ReadNoise(perlinPath);
    }
    else{

        const char* perlinPath = "asset/noiseTextures/fbmPerlin3D/data.bin";
        const char* worleyPath = "asset/noiseTextures/fbmWorley3D/data.bin";

        perlinNoise = ReadNoise(perlinPath);
        worleyNoise = ReadNoise(worleyPath);
    }
    
    
    //WorleyNoise3D wN3D;
    //rgbNoise = wN3D.GenNoise(128, 128, 128, 10.1f);

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

    float cubeB = .5f;
    glm::vec4 cubePosition(0, 6.0f, .0f,1.0);
    glm::vec3 cubeEdges(10.5, 5.5, 10.5);
    std::vector<float> Vertices = {
        -cubeEdges.x+cubePosition.x, -cubeEdges.y+cubePosition.y,  cubeEdges.z+cubePosition.z, 0, 0, 0, -1, -1, 1,
         cubeEdges.x+cubePosition.x, -cubeEdges.y+cubePosition.y,  cubeEdges.z+cubePosition.z, 1, 0, 0,  1, -1, 1,
         cubeEdges.x+cubePosition.x,  cubeEdges.y+cubePosition.y,  cubeEdges.z+cubePosition.z, 1, 1, 0,  1,  1, 1,
        -cubeEdges.x+cubePosition.x,  cubeEdges.y+cubePosition.y,  cubeEdges.z+cubePosition.z, 0, 1, 0, -1,  1, 1,
        -cubeEdges.x+cubePosition.x, -cubeEdges.y+cubePosition.y, -cubeEdges.z+cubePosition.z, 0, 0, 1, -1, -1,-1,
         cubeEdges.x+cubePosition.x, -cubeEdges.y+cubePosition.y, -cubeEdges.z+cubePosition.z, 1, 0, 1,  1, -1,-1,
         cubeEdges.x+cubePosition.x,  cubeEdges.y+cubePosition.y, -cubeEdges.z+cubePosition.z, 1, 1, 1,  1,  1,-1,
        -cubeEdges.x+cubePosition.x,  cubeEdges.y+cubePosition.y, -cubeEdges.z+cubePosition.z, 0, 1, 1, -1,  1,-1,
    };

    std::vector<unsigned int> Indices = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        1, 5, 6,
        1, 6, 2,
        0, 4, 7,
        0, 7, 3,
        0, 1, 5,
        0, 5, 4,
        3, 2, 6,
        3, 6, 7,
    };



    // 以线条的方式绘制所有的点
    VertexBuffer* AllVerticesVbPtr = new VertexBuffer(Vertices.data(), Vertices.size() * sizeof(Vertices[0]));
    AllVerticesVbPtr->Push<float>(3, false);
    AllVerticesVbPtr->Push<float>(3, false);
    AllVerticesVbPtr->Push<float>(3, false);

    //BarVbPtr->Push<float>(2, false);
    IndexBuffer* IbPtr = new IndexBuffer(Indices.data(), Indices.size());
    VertexArray* VaPtr = new VertexArray(AllVerticesVbPtr, IbPtr);
    IbPtr->Bind();
    Shader* ShaderPtr = new Shader("shader/volumetricCloud/volumetricCloud.vert", "shader/volumetricCloud/volumetricCloud.frag");
    

    // 使用 rgbNoise 来生成 3D texture
    GLuint perlinTex3D = GenTexture3D(perlinNoise.data(),128, 128, 128, GL_RGB, GL_FLOAT);
    GLuint worleyTex3D = GenTexture3D(worleyNoise.data(),128, 128, 128, GL_RGB, GL_FLOAT);



    glm::mat4 Model(1.0f);
    glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    float MoveSpeed = 1;

    glm::mat4 TIModel = glm::transpose(glm::inverse(Model));
    glm::vec3 LightColor = glm::vec3(1.0);
    glm::vec3 LightDir = glm::vec3(0.34f, 0.23, 0.67) * (-1.0f);
    glm::vec4 LightDirPos = glm::normalize(glm::vec4(-0.33, 1.623f, 1.21f, 0.0f));

    float lastTime = glfwGetTime();
    //glm::vec4 cubePosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // cube 的位置
    glm::vec4 cubeLocalX = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);       // 三个 local axis
    glm::vec4 cubeLocalY = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 cubeLocalZ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    glm::mat4 cubeDirAndPosition = glm::mat4(cubeLocalX, cubeLocalY, cubeLocalZ, cubePosition); // cube 的中心位置，还有 X、Y、Z 在 摄像机视角下的方向

    // 启用半透明效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {

        float currentTime = glfwGetTime();
        float displacement = MoveSpeed * (currentTime - lastTime);


        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);
        // render here

        GLCall(glClearColor(0.31f, 0.5f, 0.83f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        camera.updateCamera(window);

        glm::mat4 cubeDirAndPositionInView = camera.getView() * Model * cubeDirAndPosition; // 前3列是 xyz 轴在 view 参考系的值，第4列是cube的中心位置
        glm::mat4 cubeDirAndPositionInViewInv = glm::inverse(cubeDirAndPositionInView);

        //std::cout << cubeLocalXInView.x << " " << cubeLocalXInView.y << " " << cubeLocalXInView.z << std::endl;

        // 会制 提及云
        glDepthMask(GL_FALSE); // 不写深度缓冲
        ShaderPtr->Use();
        ShaderPtr->setView(camera, window);
        ShaderPtr->SetUniformMat4(TIModel, "TIModel");
        ShaderPtr->SetUniformV3(camera.getPosition(), "CameraPosition");
        ShaderPtr->SetUniform1f(currentTime, "currentTime");
        //ShaderPtr->SetUniform1f(cubeE, "cubeB");
        ShaderPtr->SetUniformV3(cubeEdges, "cubeEdges");
        glm::mat4 projectionInverse = glm::inverse(camera.getProjection(window));
        ShaderPtr->SetUniformMat4(projectionInverse, "projectionInverse");
        ShaderPtr->SetUniform1i(screenWidth, "screenWidth");
        ShaderPtr->SetUniform1i(screenHeight, "screenHeight");
        ShaderPtr->SetUniformMat4(cubeDirAndPositionInView, "cubeDirAndPosition");
        ShaderPtr->SetUniformMat4(cubeDirAndPositionInViewInv, "cubeDirAndPositionInv");
        ShaderPtr->SetTexture(perlinTex3D, GL_TEXTURE0, "FbmPerlinTex3D", GL_TEXTURE_3D);
        ShaderPtr->SetTexture(worleyTex3D, GL_TEXTURE1, "FbmWorleyTex3D", GL_TEXTURE_3D);

        ShaderPtr->SetUniformV3(LightDir, "LightDir");
        VaPtr->Bind();
        IbPtr->Bind();
        VaPtr->DrawElement(*ShaderPtr);
        //GLCall(glDrawElements(GL_LINES, IbPtr->GetCount(), GL_UNSIGNED_INT, nullptr));
        glDepthMask(GL_TRUE);  // 恢复

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete(ShaderPtr);

    glfwTerminate();
}

#endif