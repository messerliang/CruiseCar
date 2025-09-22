#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GL_Error.h"
#include "Texture.h"
#include "Camera.h"

class Shader
{
public:
	unsigned int m_shaderProgramId;

private:
	// 顶点着色器
	unsigned int m_vertexShaderId;
	unsigned int m_vertexType = GL_VERTEX_SHADER;
	std::string m_vertexShaderSource;

	// 片元着色器
	unsigned int m_fragmentShaderId;
	unsigned int m_fragmentType = GL_FRAGMENT_SHADER;
	std::string m_fragmentShaderSource;

	// 几何着色器
	unsigned int m_geometryShaderId;
	unsigned int m_geometryType = GL_GEOMETRY_SHADER;
	std::string m_geometryShaderSource;

	// tessellation control shader
	unsigned int m_tessellationControlShaderId;
	std::string m_tesselControlShaderSource; // tcs 着色器的代码

	// tesselation evaluation shader
	unsigned int m_tessellationEvalShaderId;
	std::string m_tesselEvalShaderSource; // tes 着色器的代码



	std::unordered_map<std::string, int> m_Cache;


public:
	// 三个路径：顶点着色器路径，片元着色器路径，几何着色器路径
	//Shader(const std::string& vertexCodePath, const std::string& fragmentCodePath, const std::string& geometryCodePath="");
	// 五个路径
	// 分别对应：vertex shader、frag shader、geom shader、tesel control shader、tesel evaluation shader
	Shader(
		const char* vertexCodePath, 
		const char* fragmentCodePath, 
		const char* geometryCodePath=nullptr,
		const char* tesellCSCodePath=nullptr,
		const char* tesellESCodePath=nullptr
		);

	~Shader();

	void setView(Camera& cam, GLFWwindow* window);

	int GetUniformLocation(const std::string& name="u_Color");
	void SetUniform(const float r, const float g, const float b, const float a);
	void SetUniform1f(const float val, const std::string& name) const;
	void SetUniform1i(const int val, const std::string& name) const;
	void SetUniformV3(const glm::vec3 val, const std::string& name) const;
	void SetUniformV4(const glm::vec4 val, const std::string& name) const;

	// 设置 shader 中的 uniform sampler2D tex;
	// 参数0：Texture 对象
	// 参数1：GL_TEXTURE0 ~ GL_TEXTURE31
	// 参数2：在 frag shader 代码中texture的字符串名字
	void SetTexture(Texture& tex, unsigned int glTexId, const std::string& name) const;

	// 设置 shader 中的 uniform sampler2D tex;
	// 参数0：Texture 的 id
	// 参数1：GL_TEXTURE0 ~ GL_TEXTURE31
	// 参数2：在 frag shader 代码中texture的字符串名字
	// 参数3：是 GL_TEXTURE_2D 还是 GL_TEXTURE_3D
	void SetTexture(uint32_t tex, unsigned int glTexId, const std::string& name, unsigned int texType) const;
	
	// 编译着色器
	void Compile();

	// 链接着色器
	void Link();

	void Use()const;
	void Unuse()const;

	void SetUniformMat4(const glm::mat4& trans, const std::string& uniformName)const;

private:
	std::string ParseDefaultShaderSource(const std::string& file);
	
	// 用提供的glsl代码，编译给定的 shader id
	// 参数0：unsigned int shaderId;
	// 参数1：string source
	int CompileShader(unsigned int shaderId, const std::string& source) const;
};

