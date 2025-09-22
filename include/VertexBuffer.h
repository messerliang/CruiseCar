#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "GL_Error.h"

struct LayoutElement{
	unsigned int count;			// 数据的维度，是 2维、3维还是 4 维
	unsigned int type;			// 类型if。表示是 GL_Float 还是 int 还是 byte 
	unsigned char isNormalized;	// 是否需要标准化
	unsigned int size;			// count 个元素的大小

	LayoutElement(unsigned int c = 0, unsigned int type = 0, unsigned char norm = false, unsigned int sz=0) :
		count(c), type(type), isNormalized(norm), size(sz){}
};

class VertexBuffer
{
private:

	std::vector<LayoutElement> m_layoutSets;
	unsigned int m_id=0;
	unsigned int m_stride=0;
	unsigned int m_dataSize=0;


	template<typename T>
	static unsigned int MapTypeToGLEnum() {
		if(std::is_same_v<T, float>)    return GL_FLOAT;
		if(std::is_same_v<T, int>)      return GL_INT;
		if(std::is_same_v<T, GLubyte>)  return GL_UNSIGNED_BYTE;
		
		_ASSERT(false);
		return 0;
		
	}

public:

	VertexBuffer(const void* data, unsigned int dataSize);
	VertexBuffer();
	~VertexBuffer();

	// 添加数据
	void addData(const void* data, unsigned int dataSize);
	// 更新 vertex buffer 中的数据
	void updateData(const void* data, unsigned int dataSize);


	void Bind() const;
	void Unbind() const;
	// vertexbuffer 中的数据的排布
	const std::vector<LayoutElement>& GetLayout() const;
	unsigned int GetStride() const;
	
	// 返回顶点的数量 = 数据内存大小 / 每个属性的内存大小
	unsigned int getVertexNum() const;

	unsigned int getId() const;
	
	
	template<typename T>
	void Push(unsigned int count, unsigned char isNorm=false) {
		unsigned int sz = sizeof(T);
		_ASSERTE(sz >= 1 && sz <= 4);
		sz = sz * count;
		m_layoutSets.emplace_back(count, MapTypeToGLEnum<T>(), isNorm, sz);
		m_stride += sz;
	}


};

