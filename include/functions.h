#pragma once

#include <iostream>
#include <string>
#include <random>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>      // glm::quat ���嶨��
#include <glm/gtx/quaternion.hpp>      // ����㻹Ҫ�ö������Ԫ����������ת���󡢲�ֵ�ȣ�
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <execution> 

#include "Windows.h"

// OpenGl��ʼ������
GLFWwindow* OpenGlInitiate();



std::string utf8_to_ansi(const std::string& utf8_str);

float uniformFloatDist(float lower, float upper);

void checkPointer(void* ptr, const char* msg);

// ���������	string - ͼƬ���ļ�·��
//				flipY  - ͼƬ��ת��0����x�ᷭת��1����y�ᷭת��-1��ͬʱ��x��y�ᷭת
uint8_t* opencvLoadImage(const std::string& path, int flip=111);


// �� aiMatrix4x4 ת��Ϊ glm::mat4
glm::mat4 convertToGLMMat4(const aiMatrix4x4& from);

// �� assimp �� vec3 ת��Ϊ glm �� vec3
glm::vec3 convertToGLMVec3(const aiVector3D& v);

// �� assimp ��4Ԫ��ת��Ϊ glm ����Ԫ��
glm::quat convertToGLMQuat(const aiQuaternion& q);


// ���ɸ�����Χ�������
float GenRandom(float minVal, float maxVal);


template<typename T>
void minMaxNormalize(std::vector<T>& data) {
    if (data.empty()) return;

    // ʹ�ò����㷨������С���ֵ��C++17��
    auto [min_it, max_it] = std::minmax_element(std::execution::par, data.begin(), data.end());
    T min_val = *min_it;
    T max_val = *max_it;

    // �������
    if (max_val == min_val) {
        std::fill(data.begin(), data.end(), T(0));
        return;
    }

    T range = max_val - min_val;

    // ���б任��һ��
    std::transform(std::execution::par, data.begin(), data.end(), data.begin(),
        [min_val, range](T x) { return (x - min_val) / range; });
}
