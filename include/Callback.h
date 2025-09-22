#pragma once

#include "constants.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 窗口发生变化时候的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Callback
{
};

