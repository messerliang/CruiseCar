#include "VertexArray.h"

VertexArray::VertexArray(VertexBuffer* vbObj, IndexBuffer* idObj):m_vertexBuffer(vbObj),m_indexBuffer(idObj) {
	GLCall(glGenVertexArrays(1, &m_id));
	GLCall(glBindVertexArray(m_id));
	AddBuffer(m_vertexBuffer);
}

VertexArray::VertexArray(VertexBuffer* vbObj):m_vertexBuffer(vbObj){
	
	GLCall(glGenVertexArrays(1, &m_id));
	GLCall(glBindVertexArray(m_id));
	AddBuffer(m_vertexBuffer);
}

VertexArray::VertexArray(){
	
	GLCall(glGenVertexArrays(1, &m_id));
	GLCall(glBindVertexArray(m_id));
}



VertexArray::~VertexArray() {
	GLCall(glDeleteVertexArrays(1, &m_id));
}

void VertexArray::Bind() const{
	GLCall(glBindVertexArray(m_id));
}

void VertexArray::Unbind() const {
	GLCall(glBindVertexArray(0));
}

uint32_t VertexArray::getExistAttribNum() const {
	return m_existAttributesNum;
}

void VertexArray::AddBuffer(VertexBuffer* vBuf, bool isInstance) {
	if (m_vertexBufferChannelId.count(vBuf) == 0) {
		m_vertexBufferChannelId[vBuf] = m_existAttributesNum;
	}
	// 第一个 vertex buffer，相当于是基础信息
	if (!m_vertexBuffer) {
		m_vertexBuffer = vBuf;
	}
	const std::vector<LayoutElement> layoutSets = vBuf->GetLayout();
	unsigned int offset = 0;
	this->Bind();
	vBuf->Bind();
	for (int i = 0; i < layoutSets.size(); ++i) {
		const auto& it = layoutSets[i];
		int channelId = m_existAttributesNum;
		GLCall(glEnableVertexAttribArray(channelId));
		// 参数1：属性id
		// 参数2：包含多少个数据
		// 参数3：数据类型
		// 参数4：是否标准化
		// 参数5：stride
		// 参数6：当前属性在 vertexbuffer 的偏移，单位为 byte
		GLCall(glVertexAttribPointer(channelId, it.count, it.type, it.isNormalized, vBuf->GetStride(), (void*)offset));
		offset += it.size;
		m_existAttributesNum++;

		// 当前的 vertexbuffer 数据是不是顶点数据的 instance
		if (isInstance) {
			GLCall(glVertexAttribDivisor(channelId, 1));
		}

	}

	this->Unbind();
	vBuf->Unbind();
}

void VertexArray::DrawElement(Shader& shader) const {

	Bind();
	if (!(this->m_indexBuffer))
	{
		std::cout << "index buffer not exist\n" << std::endl;
		return;
	}
	this->m_indexBuffer->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));

}
void VertexArray::DrawElementsInstanced(Shader& shader, int instancesNum) const {
	shader.Use();
	Bind();
	this->m_indexBuffer->Bind();
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr, instancesNum));
}

void VertexArray::DrawArray(Shader& shader) const {
	shader.Use();
	Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, m_vertexBuffer->getVertexNum()));
}
void VertexArray::DrawArraysInstanced(Shader& shader, int instancesNum) const {
	shader.Use();
	Bind();
	GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexBuffer->getVertexNum(), instancesNum));
}
