#pragma once

#include <iostream>
#include <string>
#include <random>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>      // glm::quat 本体定义
#include <glm/gtx/quaternion.hpp>      // 如果你还要用额外的四元数操作（如转矩阵、插值等）
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <execution> 

#include "Windows.h"

// OpenGl初始化配置
GLFWwindow* OpenGlInitiate();



std::string utf8_to_ansi(const std::string& utf8_str);

float uniformFloatDist(float lower, float upper);

void checkPointer(void* ptr, const char* msg);

// 输入参数：	string - 图片的文件路径
//				flipY  - 图片翻转，0：沿x轴翻转；1：沿y轴翻转；-1：同时沿x和y轴翻转
uint8_t* opencvLoadImage(const std::string& path, int flip=111);


// 将 aiMatrix4x4 转换为 glm::mat4
glm::mat4 convertToGLMMat4(const aiMatrix4x4& from);

// 将 assimp 的 vec3 转变为 glm 的 vec3
glm::vec3 convertToGLMVec3(const aiVector3D& v);

// 将 assimp 的4元数转变为 glm 的四元数
glm::quat convertToGLMQuat(const aiQuaternion& q);


// 生成给定范围的随机数
float GenRandom(float minVal, float maxVal);


template<typename T>
void minMaxNormalize(std::vector<T>& data) {
    if (data.empty()) return;

    // 使用并行算法查找最小最大值（C++17）
    auto [min_it, max_it] = std::minmax_element(std::execution::par, data.begin(), data.end());
    T min_val = *min_it;
    T max_val = *max_it;

    // 避免除零
    if (max_val == min_val) {
        std::fill(data.begin(), data.end(), T(0));
        return;
    }

    T range = max_val - min_val;

    // 并行变换归一化
    std::transform(std::execution::par, data.begin(), data.end(), data.begin(),
        [min_val, range](T x) { return (x - min_val) / range; });
}
