#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Camera.h"

extern int screenWidth;
extern int screenHeight;

//     extern bool keys[1024];
//     extern bool mouseKeys[1024];
//     
//     // 摄像机相关的参数
//     extern glm::vec3 cameraPos;
//     extern glm::vec3 cameraDirection;
//     extern glm::vec3 cameraUp;
//     
//     // 摄像机旋转相关的参数，单位为度
//     extern float viewPitch;
//     extern float viewYaw;
//     
//     // 鼠标位置
//     extern bool resetMousePos;
//     extern double mouseX;
//     extern double mouseY;
//     
//     // 与帧速率相关的配置
//     extern float deltaTime;
//     extern float lastFrame;
//     
//     
//     // 设置缩放
//     extern float fov;

// 摄像机对象
extern Camera camera;