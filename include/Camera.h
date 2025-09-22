#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




class Camera
{
private:
	GLFWwindow* m_window;

	// 摄像机相关的参数
	glm::vec3 m_cameraPos = glm::vec3(0.0f, 2.0f, 1.0f);
	glm::vec3 m_cameraDirection = glm::vec3(0.0f, -0.0f, -1.0f);
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


	glm::vec3 m_cameraTarget = glm::vec3(0.0, 0.0f, 0.0);

	// 摄像机移动速度
	float m_speed = 3.0f;

	// 摄像机旋转相关的参数，单位为度
	float m_viewPitch = 5.0f;		// 俯仰角
	float m_viewYaw = 0.0f;			// 偏航角

	// 鼠标位置
	bool m_resetMousePos = true;
	double m_mouseX = 0.0f;
	double m_mouseY =  0.0f;

	// 鼠标旋转
	bool m_startRotateFlag = false;
	bool m_updateRotateFlag = false;
	float m_rotateAnchorX = 0.0f;
	float m_rotateAnchorY = 0.0f;

	// 与帧速率相关的配置
	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;

	// 设置缩放
	float m_fov = 30.0f;

	/// <summary>
	/// 鼠标和键盘记录
	/// </summary>
	bool m_keys[1024] = { false };
	bool m_mouseKeys[1024] = { false };

	// 对外显示的参数


public:
	Camera();


	void bindCallbackToWindow(GLFWwindow* window);
	void updateCamera(GLFWwindow* window);

	// 根据俯仰角更新摄像机的指向
	void updateCameraDirection();
	void updateCameraPosition();
	void updateCameraRotation(GLFWwindow* window);

	glm::mat4 getView();
	glm::mat4 getProjection(GLFWwindow* window, bool useProj=true);
	glm::vec3 getPosition() const;

	void SetPosition(const glm::vec3& Position);

	friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	friend void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);


	
};

