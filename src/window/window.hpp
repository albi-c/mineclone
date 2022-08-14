#pragma once

#include "event.hpp"
#include "camera/camera.hpp"
#include "lib/opengl.hpp"
#include "graphics/render/renderer.hpp"
#include "event/event.hpp"
#include "graphics/gui/imgui.hpp"

class Window {
public:
    Renderer renderer;
    Camera camera;

    Window();
    Window(Window* other);
    ~Window();

    bool update();
    void render_start();
    void render_end();

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
