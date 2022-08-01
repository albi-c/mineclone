#pragma once

#include <bitset>

#include "util.hpp"
#include "consts.hpp"
#include "camera.hpp"

#include "graphics/opengl.hpp"
#include "graphics/renderer.hpp"
#include "event/event.hpp"

class Window {
public:
    Renderer renderer;
    Camera camera;

    Window();
    ~Window();

    bool update();
    void render();

    bool get_key(int key);

    void close();

    void grab_mouse(bool grabbed);

private:
    GLFWwindow* window;
    int width = 1, height = 1;
    double mouseX = 0.0, mouseY = 0.0;
    double dt = 0.0, last_time = 0.0;
    double fps_dt = 0.0, fps_last_time = 0.0;

    static void callback_framebuffer_resize(GLFWwindow* glfw_window, int width, int height);
    void _callback_framebuffer_resize(int width, int height);

    static void callback_mouse_move(GLFWwindow* glfw_window, double x, double y);
    void _callback_mouse_move(double x, double y);

    static void callback_mouse_click(GLFWwindow* glfw_window, int button, int action, int mods);
    void _callback_mouse_click(int button, int action, int mods);

    static void callback_key_press(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
    void _callback_key_press(int key, int scancode, int action, int mods);
};
