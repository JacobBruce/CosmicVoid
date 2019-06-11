#pragma once
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Keyboard.h"
#include "Mouse.h"

class GLFW
{
public:
    static KeyboardServer kServ;
    static MouseServer mServ;

    static const char* GetInputStr(const int key)
    {
        return glfwGetKeyName(key, 0);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action)
        {
            case GLFW_PRESS:
                kServ.OnKeyPressed(key);
                break;
            case GLFW_RELEASE:
                kServ.OnKeyReleased(key);
                break;
            case GLFW_REPEAT:
                break;
            default:
                break;
        }
    }

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        mServ.OnMouseMove((float)xpos, (float)ypos);
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        switch (action) {
        case GLFW_PRESS:
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                mServ.OnRightPressed();
            } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mServ.OnLeftPressed();
            }
            break;
        case GLFW_RELEASE:
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                mServ.OnRightReleased();
            } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mServ.OnLeftReleased();
            }
            break;
        default: break;
        }
    }

    static void cursor_enter_callback(GLFWwindow* window, int entered)
    {
        if (entered) {
            mServ.OnMouseEnter();
        } else {
            mServ.OnMouseLeave();
        }
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (yoffset > 0) {
            mServ.OnWheelUp(xoffset, yoffset);
        } else if (yoffset < 0) {
            mServ.OnWheelDown(xoffset, yoffset);
        }
    }

    static void window_close_callback(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    static void error_callback(int error, const char* description)
    {
        std::cerr << "Error: " << description << std::endl;
    }

    static void error_exit(GLFWwindow* window=nullptr, bool wait=true)
    {
        if (window != nullptr) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
        if (wait) { std::cin.get(); }
        exit(EXIT_FAILURE);
    }
};
