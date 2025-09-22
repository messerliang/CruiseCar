
#include "Texture.h"
#include "GL_Error.h"

#include "functions.h"

#include <windows.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

Texture::Texture(const std::string& imgPath,  int flip): m_textureId(-1){
    m_imagePath = utf8_to_ansi(imgPath);
    

    // ��ȡͼƬ
    if (0 != ReadImage(flip)) {

        //int len = MultiByteToWideChar(CP_UTF8, 0, imgPath.c_str(), -1, NULL, 0);
        //std::wstring ws(len, L'\0');
        //MultiByteToWideChar(CP_UTF8, 0, imgPath.c_str(), -1, &ws[0], len);

        //// �Ƴ�ĩβ�� null terminator
        //ws.resize(wcslen(ws.c_str()));
        //std::wcout << ws << " read failed\n";
        std::cout << utf8_to_ansi(imgPath) << "read failed\n"; 
        m_textureId = -1;
        return;
    }
    else {
        std::cout << "success: " << utf8_to_ansi(imgPath) << std::endl;
        
    }

    // ��������:������ظ���ʽ���Լ� Mimap
    BasicSet();

    // ��������
    GLCall(glGenTextures(1, &m_textureId));

    // ��
    Bind2D();

    // ����ͼ������
    SetImagData2D();



}

Texture::~Texture() {
    this->UnBind2D();


    if (m_imageData) {
        free(m_imageData);
    }
    
}

int Texture::GetWidth()const {
    return m_width;
}

int Texture::GetHeight()const {
    return m_height;
}

int Texture::GetChannel()const {
    return m_channel;
}

void Texture::SetImagData2D() const {
    if (m_imageData) {
        if (ImageType::PNG == m_imageType) {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_imageData));
        }
        else {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData));
        }
        
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else {
        std::cout << "image data is empty\n";
    }
    
    
}

void Texture::Bind2D() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, m_textureId));
}
void Texture::UnBind2D() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

unsigned int Texture::GetId()const {
    return m_textureId;
}

void Texture::BasicSet() {
    if (m_imageType == ImageType::PNG) {
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }
    else {
        // ����Ϊ�ظ���ʱ�����������ڱ�Ե��λ�ú���һ��ͼ����в���
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
    }
    

    //GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));      // Ϊ������Сʱ�����ò�����ʽΪ�����
    //GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));       // ����Ŵ�ʹ�����Բ���
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    

}

Texture::ImageType Texture::GetImageType(const std::string& file) const{
    std::ifstream fd(file, std::ios::binary);
    if (!fd) {
        std::cerr << "can not open: " << file << "\n";
        return ImageType::UNKNOW;
    }
    unsigned char header[8];
    fd.read(reinterpret_cast<char*>(header), sizeof(header));
    // �ж� PNG
    if (header[0] == 0x89 &&
        header[1] == 0x50 && // P
        header[2] == 0x4E && // N
        header[3] == 0x47 && // G
        header[4] == 0x0D &&
        header[5] == 0x0A &&
        header[6] == 0x1A &&
        header[7] == 0x0A) {
        return ImageType::PNG;
    }

    // �ж� JPEG
    if (header[0] == 0xFF &&
        header[1] == 0xD8 &&
        header[2] == 0xFF) {
        return ImageType::JPEG;
    }
    // �ж� BMP
    if (header[0] == 0x42 && header[1] == 0x4D) { // 'B''M'
        return ImageType::BMP;
    }
    return ImageType::UNKNOW;
}

int Texture::ReadImage(int flip) {

    //if(flipY){ stbi_set_flip_vertically_on_load(true); }
    //
    //m_imageData = stbi_load(m_imagePath.c_str(), &m_width, &m_height, &m_channel, 0);
    //if (!m_imageData) {
    //    return -1;
    //}
    m_imageType = GetImageType(m_imagePath);
    if (ImageType::UNKNOW == m_imageType) {
        return -1;
    }


    // �ص� opencv �� INFO ��ӡ
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    // ���� cv::IMREAD_UNCHANGED ֻ�� png ��ʱ�����ã���Ҫ���ڱ��� alpha ͨ��
    cv::Mat img = cv::imread(m_imagePath, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        
        return -1;
    }
    // �仯Ϊ RGB �� RGBA
    if (ImageType::BMP == m_imageType || ImageType::JPEG == m_imageType) {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
    } else{
        cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
    }
    
    if (flip >= -1 && flip <=1) {
        cv::flip(img, img, flip);
        std::cout << "flip: " << flip << std::endl;
    }
    size_t sz = img.total() * img.elemSize();
    m_imageData = (unsigned char*)new char[sz];
    std::memcpy(m_imageData, img.data, sz);
    m_width = img.cols;
    m_height = img.rows;
    m_channel = img.channels();

    return 0;
}

