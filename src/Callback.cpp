#include "Callback.h"
#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    //std::cout << "pos: " << camera.m_cameraPos.x << " " << camera.m_cameraPos.y << " " << camera.m_cameraPos.z
    //    << " look at:" << camera.m_cameraDirection.x << " " << camera.m_cameraDirection.y << " " << camera.m_cameraDirection.z << std::endl;
    
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            camera.m_keys[key] = true;
        else if (action == GLFW_RELEASE)
            camera.m_keys[key] = false;
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < 1024) {
        if (action == GLFW_PRESS) {
            camera.m_mouseKeys[button] = true;
            if (GLFW_MOUSE_BUTTON_LEFT == button) {
                camera.m_startRotateFlag = true;
            }
        }
        else if (action == GLFW_RELEASE) {
            camera.m_mouseKeys[button] = false;
            if (GLFW_MOUSE_BUTTON_MIDDLE == button) {
                camera.m_resetMousePos = true;
                
            }
            if (GLFW_MOUSE_BUTTON_LEFT == button) {
                camera.m_updateRotateFlag = true;

            }
        }
    }
}
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    float max_fov = 160.0f, min_fov = 1.0f;
    if (camera.m_fov >= min_fov && camera.m_fov <= max_fov) {
        camera.m_fov -= yoffset;
    }

    if (camera.m_fov < min_fov) { camera.m_fov = min_fov; }
    if (camera.m_fov > max_fov) { camera.m_fov = max_fov; }

    //std::cout << "scroll adjust: " << yoffset << ", fov: " << camera.m_fov << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // 更新视口，确保渲染到整个窗口
    glViewport(0, 0, width, height);


    //std::cout << "width: " << width << ", height: " << height << std::endl;
    
}