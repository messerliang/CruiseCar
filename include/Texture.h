#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cstdlib>
#include <iostream>

class Texture
{
public:

	enum ImageType {
		PNG,
		JPEG,
		JPG,
		BMP,
		UNKNOW
	};
private:
	unsigned int m_textureId;
	std::string m_imagePath;
	ImageType m_imageType;
	uint32_t m_gl_texture;
	int m_width;
	int m_height;
	int m_channel;
	unsigned char* m_imageData = nullptr;

	// 读取数据，默认垂直翻转
	int ReadImage(int flip);

public:
	// 输入参数：	string - 图片的文件路径
	//				flipY  - 图片翻转，0：沿x轴翻转；1：沿y轴翻转；-1：同时沿x和y轴翻转
	Texture(const std::string& imgPath="", int flipY = 111);
	Texture(const Texture& other):
		m_imageType(other.m_imageType),
		m_textureId(other.m_textureId),
		m_imagePath(other.m_imagePath),
		m_width(other.m_width),
		m_height(other.m_height),
		m_channel(other.m_channel)
	{
		if (other.m_imageData) {
			uint32_t size = m_width * m_height * m_channel;
			m_imageData = new unsigned char[size];
			std::memcpy(m_imageData, other.m_imageData, size);
			//std::copy(other.m_imageData, other.m_imageData + size, m_imageData);
		}
		else {
			std::cout << "copy src has no img data\n";
			m_imageData = nullptr;
		}
	}
	Texture& operator=(const Texture& other) {
		if (this == &other) {
			return *this;
		}
		if (m_imageData) {
			free(m_imageData);
		}
		m_imageType = other.m_imageType;
		m_textureId = other.m_textureId;
		m_imagePath = other.m_imagePath;
		m_width = other.m_width;
		m_height = other.m_height;
		m_channel = other.m_channel;
		uint32_t size = m_width * m_height * m_channel;
		m_imageData = new unsigned char[size];
		std::copy(other.m_imageData, other.m_imageData + size, m_imageData);
		return *this;
	}
	~Texture();

	void BasicSet();
	unsigned int GetId() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetChannel() const;
	void Bind2D() const;
	void UnBind2D() const;
	void SetImagData2D() const;

	ImageType GetImageType(const std::string& file) const;
};

