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
	// ������ɫ��
	unsigned int m_vertexShaderId;
	unsigned int m_vertexType = GL_VERTEX_SHADER;
	std::string m_vertexShaderSource;

	// ƬԪ��ɫ��
	unsigned int m_fragmentShaderId;
	unsigned int m_fragmentType = GL_FRAGMENT_SHADER;
	std::string m_fragmentShaderSource;

	// ������ɫ��
	unsigned int m_geometryShaderId;
	unsigned int m_geometryType = GL_GEOMETRY_SHADER;
	std::string m_geometryShaderSource;

	// tessellation control shader
	unsigned int m_tessellationControlShaderId;
	std::string m_tesselControlShaderSource; // tcs ��ɫ���Ĵ���

	// tesselation evaluation shader
	unsigned int m_tessellationEvalShaderId;
	std::string m_tesselEvalShaderSource; // tes ��ɫ���Ĵ���



	std::unordered_map<std::string, int> m_Cache;


public:
	// ����·����������ɫ��·����ƬԪ��ɫ��·����������ɫ��·��
	//Shader(const std::string& vertexCodePath, const std::string& fragmentCodePath, const std::string& geometryCodePath="");
	// ���·��
	// �ֱ��Ӧ��vertex shader��frag shader��geom shader��tesel control shader��tesel evaluation shader
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

	// ���� shader �е� uniform sampler2D tex;
	// ����0��Texture ����
	// ����1��GL_TEXTURE0 ~ GL_TEXTURE31
	// ����2���� frag shader ������texture���ַ�������
	void SetTexture(Texture& tex, unsigned int glTexId, const std::string& name) const;

	// ���� shader �е� uniform sampler2D tex;
	// ����0��Texture �� id
	// ����1��GL_TEXTURE0 ~ GL_TEXTURE31
	// ����2���� frag shader ������texture���ַ�������
	// ����3���� GL_TEXTURE_2D ���� GL_TEXTURE_3D
	void SetTexture(uint32_t tex, unsigned int glTexId, const std::string& name, unsigned int texType) const;
	
	// ������ɫ��
	void Compile();

	// ������ɫ��
	void Link();

	void Use()const;
	void Unuse()const;

	void SetUniformMat4(const glm::mat4& trans, const std::string& uniformName)const;

private:
	std::string ParseDefaultShaderSource(const std::string& file);
	
	// ���ṩ��glsl���룬��������� shader id
	// ����0��unsigned int shaderId;
	// ����1��string source
	int CompileShader(unsigned int shaderId, const std::string& source) const;
};

