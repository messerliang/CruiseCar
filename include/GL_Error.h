#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


void GLClearError();

bool GLCheckError(const char* function, const char* file, int line);


#define GLCall(x) \
    GLClearError(); \
    x; \
    GLCheckError(#x, __FILE__, __LINE__)

