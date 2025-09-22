#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GL_Error.h"

#include <unordered_map>

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class VertexArray
{


private:
	unsigned int m_id;
	VertexBuffer* m_vertexBuffer=nullptr;
	IndexBuffer* m_indexBuffer=nullptr;
	std::unordered_map<VertexBuffer*, int> m_vertexBufferChannelId;		//  vertexBuffer 的属性在着色器上的通道开始的位置
	uint32_t m_existAttributesNum = 0;
public:



	// 需要使用 vertexBuffer 和 IndexBuffer 来创建 vertexArray
	VertexArray(VertexBuffer* vbObj, IndexBuffer* idObj);
	VertexArray(VertexBuffer* vbObj);
	VertexArray();

	~VertexArray();
	
	void Bind()const;
	void Unbind()const;

	// vertexarray添加一个属性
	void AddBuffer(VertexBuffer* vBuf, bool isInstance = false);
	// 更新某个通道的 vertex buffer 数据
	//void updateBufferData(VertexBuffer* vBuf, int channelId);
	
	// 使用 index buffer 来绘制图形
	void DrawElement(Shader& shader) const;
	// 使用 index buffer 来绘制图形实例
	void DrawElementsInstanced(Shader& shader, int instancesNum) const;
	
	// 绘制顶点数组
	void DrawArray(Shader& shader) const;
	// 绘制实例
	void DrawArraysInstanced(Shader& shader, int instancesNum) const;
	
	uint32_t getExistAttribNum() const;
};

