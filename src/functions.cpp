#include "functions.h"



GLFWwindow* OpenGlInitiate()
{

	if (!glfwInit()) {
		std::cout << "glfwInit failed\n";
		return nullptr;
	}

	// 读取当前显示器的信息
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	std::cout << "screen info:\n";
	std::cout << "width: " << mode->width << ", height: " << mode->height << std::endl;
	std::cout << "refresh rate: " << mode->refreshRate << std::endl;
	std::cout << "RGB bits: " << mode->redBits << ", " << mode->greenBits << ", " << mode->blueBits << std::endl;

	// 配置上下文
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	int screenWidth = 800, screenHeight = 800;
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "depth test", nullptr, nullptr);
	if (!window) {
		std::cerr << "window create failed\n";
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	// 初始化 GLEW（必须在上下文创建后）
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		return nullptr;
	}
	glEnable(GL_DEPTH_TEST);   // 开启深度测试
	glDepthFunc(GL_LESS);      // 指定深度比较方式（默认是 GL_LESS）

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
	glEnable(GL_MULTISAMPLE);

	return window;
}

std::string utf8_to_ansi(const std::string& utf8_str) {
    // 第一步：UTF-8 转换为 UTF-16（宽字符）
    int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
    if (wide_len == 0) return "";

    std::wstring wide_str(wide_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_str[0], wide_len);

    // 第二步：UTF-16（宽字符） 转 ANSI（GBK 编码）
    int ansi_len = WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (ansi_len == 0) return "";

    std::string ansi_str(ansi_len, 0);
    WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, &ansi_str[0], ansi_len, nullptr, nullptr);

    return ansi_str;
}

float uniformFloatDist(float lower, float upper) {
    static std::random_device randomDevice;
    static std::mt19937 gen(randomDevice());

    std::uniform_real_distribution<> floatDist(lower, upper);
    return floatDist(gen);
}

void checkPointer(void* ptr, const char* msg) {
    if (!ptr) {
        std::cout << msg << std::endl;
        exit(-1);
    }

}

uint8_t* opencvLoadImage(const std::string& path, int flip) {
    return nullptr;
}


// 将 aiMatrix4x4 转换为 glm::mat4
glm::mat4 convertToGLMMat4(const aiMatrix4x4& from) {
    glm::mat4 to;

    // Assimp 是按行主序（row-major），GLM 默认也是 row-major（如果不强制列主序）
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

    return to;
}

glm::vec3 convertToGLMVec3(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

glm::quat convertToGLMQuat(const aiQuaternion& q) {
    // GLM 的构造函数是 glm::quat(w, x, y, z)
    return glm::quat(q.w, q.x, q.y, q.z);
}


float GenRandom(float minVal, float maxVal)
{
	std::random_device rd;

	// 用 Mersenne Twister 算法作为生成器
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distReal(minVal, maxVal);
	//return minVal + ((float)rand()) / RAND_MAX * (maxVal - minVal);
	return distReal(gen);
}
