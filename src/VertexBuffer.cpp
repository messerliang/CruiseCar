#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int dataSize):m_stride(0){
    addData(data, dataSize);
}

VertexBuffer::VertexBuffer():m_stride(0) {
    
}

void VertexBuffer::addData(const void* data, unsigned int dataSize) {
    if (m_dataSize) {
        std::cout << "current VB has data\n";
        exit(1);
    }
    GLCall(glGenBuffers(1, &m_id));
    m_dataSize = dataSize;
    this->Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW));

}

void VertexBuffer::updateData(const void* data, unsigned int dataSize) {
    if (!m_dataSize) {
        std::cout << "current buffer have not been created\n";
        exit(-1);
    }
    else {
        this->Bind();
        // 需要分配更大的缓冲区
        if (dataSize > m_dataSize) {
            glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
        }
        else {
            GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data));
        }
        m_dataSize = dataSize;
        
    }

}

VertexBuffer::~VertexBuffer() {

    GLCall(glDeleteBuffers(1, &m_id));
}

void VertexBuffer::Bind() const{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void VertexBuffer::Unbind() const{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


const std::vector<LayoutElement>& VertexBuffer::GetLayout() const {
    return m_layoutSets;
}


unsigned int VertexBuffer::GetStride() const {
    return m_stride;
}


unsigned int VertexBuffer::getVertexNum() const {
    return m_dataSize / m_stride;
}



unsigned int VertexBuffer::getId() const {
    return m_id;
}