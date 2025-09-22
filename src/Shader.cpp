#include "Shader.h"


//Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode, const std::string& geometryCodePath):m_vertexShaderId(0),
//m_fragmentShaderId(0), m_geometryShaderId(0) {
//
//
//    GLCall(m_vertexShaderSource = ParseDefaultShaderSource(vertexCode));
//    GLCall(m_fragmentShaderSource = ParseDefaultShaderSource(fragmentCode));
//    GLCall(m_geometryShaderSource = ParseDefaultShaderSource(geometryCodePath));
//
//    GLCall(m_vertexShaderId = glCreateShader(m_vertexType));
//    GLCall(m_fragmentShaderId = glCreateShader(m_fragmentType));
//    if (m_geometryShaderSource.size()) {
//        GLCall(m_geometryShaderId = glCreateShader(m_geometryType));
//    }
//    
//
//    GLCall(m_shaderProgramId = glCreateProgram());
//    Compile();
//    
//    Link();
//
//
//}

Shader::Shader(const char* vertexCodePath, const char* fragmentCodePath, const char* geometryCodePath, const char* tesellCSCodePath, const char* tesellESCodePath)
{
    if (vertexCodePath)
    {
        m_vertexShaderSource = ParseDefaultShaderSource(vertexCodePath);
        GLCall(m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER));
    }
    if (fragmentCodePath)
    {
        m_fragmentShaderSource = ParseDefaultShaderSource(fragmentCodePath);
        GLCall(m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER));
    }
    if (geometryCodePath)
    {
        m_geometryShaderSource = ParseDefaultShaderSource(geometryCodePath);
        GLCall(m_geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER));
    }
    if (tesellCSCodePath)
    {
        m_tesselControlShaderSource = ParseDefaultShaderSource(tesellCSCodePath);
        GLCall(m_tessellationControlShaderId = glCreateShader(GL_TESS_CONTROL_SHADER));
    }
    if (tesellESCodePath)
    {
        m_tesselEvalShaderSource = ParseDefaultShaderSource(tesellESCodePath);
        GLCall(m_tessellationEvalShaderId = glCreateShader(GL_TESS_EVALUATION_SHADER));
    }

    GLCall(m_shaderProgramId = glCreateProgram());
    Compile();
    Link();
}

Shader::~Shader() {
    glDeleteShader(m_vertexShaderId);
    glDeleteShader(m_fragmentShaderId);
    if (m_geometryShaderId) {
        glDeleteShader(m_geometryShaderId);
    }
    glDeleteProgram(m_shaderProgramId);
}

std::string Shader::ParseDefaultShaderSource(const std::string& file) {
    if (file.size() == 0) {
        return "";
    }
    std::ifstream stream(file);

    if (!stream.is_open()) {
        std::cout << file << "open failed\n";
        return "";
    }

    std::stringstream ss;

    std::string line;

    while (std::getline(stream, line)) {
        ss << line << "\n";
    }

    return ss.str();
}

void Shader::Compile() {
    CompileShader(m_vertexShaderId, m_vertexShaderSource);
    CompileShader(m_fragmentShaderId, m_fragmentShaderSource);
    if (m_geometryShaderSource.size()) {
        CompileShader(m_geometryShaderId, m_geometryShaderSource);
    }
    if (m_tesselControlShaderSource.size()) {
        CompileShader(m_tessellationControlShaderId, m_tesselControlShaderSource);
    }
    if (m_tesselEvalShaderSource.size()) {
        CompileShader(m_tessellationEvalShaderId, m_tesselEvalShaderSource);
    }
    
}

void Shader::Link()
{
    GLCall(glAttachShader(m_shaderProgramId, m_vertexShaderId));
    GLCall(glAttachShader(m_shaderProgramId, m_fragmentShaderId));
    if (m_geometryShaderSource.size()) {
        GLCall(glAttachShader(m_shaderProgramId, m_geometryShaderId));
    }
    if (m_tesselControlShaderSource.size()) {
        GLCall(glAttachShader(m_shaderProgramId, m_tessellationControlShaderId));
    }
    if (m_tesselEvalShaderSource.size()) {
        GLCall(glAttachShader(m_shaderProgramId, m_tessellationEvalShaderId));
    }

    GLCall(glLinkProgram(m_shaderProgramId));
    GLCall(glValidateProgram(m_shaderProgramId));
}

int Shader::CompileShader(unsigned int shaderId, const std::string& source)const {


    //unsigned int id = m_fragmentShaderId;
    //if (type == m_vertexType) {
    //    id = m_vertexShaderId;
    //}
    //else if (type == m_geometryType) {
    //    id = m_geometryShaderId;
    //}
    const char* src = source.c_str();
    // 参数1：shader id
    // 参数2：有多少个 shader source code
    // 参数3：这些 source code 数组的首地址
    // 参数4：每个 source code 的长度
    glShaderSource(shaderId, 1, &src, NULL);

    glCompileShader(shaderId);

    // 查看是否成功compile
    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* msg = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(shaderId, length, &length, msg);

        std::cout << "failed to compile shader id " << shaderId << ": " << msg << std::endl;
        glDeleteShader(shaderId);
        return -1;
    }

    return 0;

}

void Shader::setView(Camera& cam, GLFWwindow* window) {
    this->Use();
    glm::mat4 model(1.0f);
    glm::mat4 view = cam.getView();
    glm::mat4 projection = cam.getProjection(window);

    SetUniformMat4(model, "model");
    SetUniformMat4(view, "view");
    SetUniformMat4(projection, "projection");

}

int Shader::GetUniformLocation(const std::string& name) {
    if (m_Cache.find(name) != m_Cache.end()) {
        return m_Cache[name];
    }
    int location = glGetUniformLocation(m_shaderProgramId, name.c_str());
    //_ASSERT((location != -1));
    m_Cache[name] = location;
    return location;
}

void Shader::SetUniform(const float r, const float g, const float b, const float a) {

    // 从 CPU 侧设置材质
    GLCall(glUniform4f(GetUniformLocation(), r, g, b, a));


}

void Shader::SetUniform1f(const float val, const std::string& name) const{
    GLCall(unsigned int loc = glGetUniformLocation(m_shaderProgramId, name.c_str()));
    GLCall(glUniform1f(loc,  val));
    
}

void Shader::SetUniform1i(const int val, const std::string& name) const
{
    GLCall(unsigned int loc = glGetUniformLocation(m_shaderProgramId, name.c_str()));
    GLCall(glUniform1i(loc, val));
}

void Shader::SetUniformV3(const glm::vec3 val, const std::string& name) const {

    GLCall(unsigned int loc = glGetUniformLocation(m_shaderProgramId, name.c_str()));
    glUniform3f(loc, val.x, val.y, val.z);

}
void Shader::SetUniformV4(const glm::vec4 val, const std::string& name) const {
    GLCall(unsigned int loc = glGetUniformLocation(m_shaderProgramId, name.c_str()));
    glUniform4f(loc, val.x, val.y, val.z, val.w);
}

void Shader::SetUniformMat4(const glm::mat4& trans, const std::string& uniformName)const {

    GLCall(unsigned int transformLoc = glGetUniformLocation(m_shaderProgramId, uniformName.c_str()));
    // 参数1：glsl里面的 transform 的位置
    // 参数2：变换矩阵的数量
    // 参数3：是否转置
    // 参数4：符合 opengl 要求的矩阵数据
    GLCall(glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans)));

}
void Shader::SetTexture(Texture& tex, unsigned int gl_textureId, const std::string& name) const {
    if (!(gl_textureId >= GL_TEXTURE0 && gl_textureId <= GL_TEXTURE31)) {
        std::cout << "error texId, must in GL_TEXTURE0(" << GL_TEXTURE0 << ") ~ GL_TEXTURE31(" << GL_TEXTURE31 << ")\n";
    }

    GLCall(glActiveTexture(gl_textureId));
    tex.Bind2D();
    GLCall(glUniform1i(glGetUniformLocation(m_shaderProgramId, name.c_str()), gl_textureId - GL_TEXTURE0));

}

void Shader::SetTexture(uint32_t texId, unsigned int gl_textureId, const std::string& name, unsigned int texType) const {
    if (!(gl_textureId >= GL_TEXTURE0 && gl_textureId <= GL_TEXTURE31)) {
        std::cout << "error texId, must in GL_TEXTURE0(" << GL_TEXTURE0 << ") ~ GL_TEXTURE31(" << GL_TEXTURE31 << ")\n";
    }

    GLCall(glActiveTexture(gl_textureId));
    if (GL_TEXTURE_2D == texType)
    {
        GLCall(glBindTexture(GL_TEXTURE_2D, texId));
    }
    else {
        GLCall(glBindTexture(GL_TEXTURE_3D, texId));
    }
    
    GLCall(glUniform1i(glGetUniformLocation(m_shaderProgramId, name.c_str()), gl_textureId - GL_TEXTURE0));

}



void Shader::Use() const {
    GLCall(glUseProgram(m_shaderProgramId));
}

void Shader::Unuse() const {
    GLCall(glUseProgram(0));
}


