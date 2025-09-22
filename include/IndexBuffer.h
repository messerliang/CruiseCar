#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GL_Error.h"


class IndexBuffer
{
private:
	unsigned int m_id=0;
	unsigned int m_count=0;
public:
	//
	// 参数1：index 数组
	// 参数2：数组包含多少个元素
	//
	IndexBuffer(const unsigned int* index, unsigned int count);
	IndexBuffer();
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	// 返回 IndexArray 有多少个下标元素
	unsigned int GetCount() const;

	// 更新 Index 数组
	void updatteData(const unsigned int* index, unsigned int count);
};

