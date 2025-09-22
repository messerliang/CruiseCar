#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(const unsigned int* index, unsigned int count):m_count(count) {
    GLCall(glGenBuffers(1, &m_id));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), index, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer():m_id(0), m_count(0) {

}


IndexBuffer::~IndexBuffer() {

}
void IndexBuffer::Bind() const{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}
void IndexBuffer::Unbind() const{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int IndexBuffer::GetCount() const {
    return m_count;
}

void IndexBuffer::updatteData(const unsigned int* index, unsigned int count) {
    if (!m_id) {
        std::cout << "index buffer has not been created\n";
        exit(-1);
    }
    
    this->Bind();
    if (m_count < count) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*count, index, GL_DYNAMIC_DRAW);
    }
    else {
        GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_count, index));
    }
    
    m_count = count;
}