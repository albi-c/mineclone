#include "window.hpp"

#include "gl_debug.inl"

Window::Window()
    : camera(45.0f) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if OPENGL_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    #endif
    glfwWindowHint(GLFW_SAMPLES, 16);

    window = glfwCreateWindow(1280, 720, "MineClone", NULL, NULL);
    if (window == NULL) {
        raiseError("Couldn't create window");
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        raiseError("Couldn't load OpenGL");
    }

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)(&Window::callback_framebuffer_resize));

    glfwSetCursorPosCallback(window, (GLFWcursorposfun)(&Window::callback_mouse_move));
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)(&Window::callback_mouse_click));
    glfwSetKeyCallback(window, (GLFWkeyfun)(&Window::callback_key_press));

    glfwMaximizeWindow(window);
    glfwPollEvents();

    renderer.init(&camera, width, height);
    renderer.resize(1, 1);
    camera.resize(1, 1);

    #if OPENGL_DEBUG
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    #endif

    glEnable(GL_MULTISAMPLE);
}

Window::~Window() {
    glfwDestroyWindow(window);
}

bool Window::update() {
    glfwPollEvents();

    char movement = 0;
    if (get_key(GLFW_KEY_W))
        movement |= (int)CameraMoveDirection::FORWARD;
    if (get_key(GLFW_KEY_S))
        movement |= (int)CameraMoveDirection::BACKWARD;
    if (get_key(GLFW_KEY_A))
        movement |= (int)CameraMoveDirection::LEFT;
    if (get_key(GLFW_KEY_D))
        movement |= (int)CameraMoveDirection::RIGHT;
    if (get_key(GLFW_KEY_SPACE))
        movement |= (int)CameraMoveDirection::UP;
    if (get_key(GLFW_KEY_LEFT_SHIFT))
        movement |= (int)CameraMoveDirection::DOWN;
    camera.move(movement, dt * 40);

    double time = glfwGetTime();
    dt = time - last_time;
    last_time = time;

    return !glfwWindowShouldClose(window);
}
void Window::render() {
    glfwSwapBuffers(window);
    
    renderer.render();

    double time = glfwGetTime();
    fps_dt = time - fps_last_time;
    fps_last_time = time;
}

bool Window::get_key(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void Window::close() {
    glfwSetWindowShouldClose(window, true);
}

void Window::grab_mouse(bool grabbed) {
    if (grabbed)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::callback_framebuffer_resize(GLFWwindow* glfw_window, int width, int height) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_framebuffer_resize(width, height);
}
void Window::_callback_framebuffer_resize(int width, int height) {
    this->width = width;
    this->height = height;

    renderer.resize(width, height);
    camera.resize(width, height);

    EventManager::fire(EventFramebufferResize{width, height});
}

void Window::callback_mouse_move(GLFWwindow* glfw_window, double x, double y) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_mouse_move(x, y);
}
void Window::_callback_mouse_move(double x, double y) {
    double dx = x - mouseX;
    double dy = mouseY - y;

    camera.rotate(dx, dy);

    mouseX = x;
    mouseY = y;

    EventManager::fire(EventMouseMove{x, y});
}

void Window::callback_mouse_click(GLFWwindow* glfw_window, int button, int action, int mods) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_mouse_click(button, action, mods);
}
void Window::_callback_mouse_click(int button, int action, int mods) {
    // if (action == GLFW_PRESS)
    //     Event{EventType::MouseClick, (void*)(long)button}.fire();
    EventManager::fire(EventMouseClick{button, (int)mouseX, (int)mouseY, action, mods});
}

void Window::callback_key_press(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_key_press(key, scancode, action, mods);
}
void Window::_callback_key_press(int key, int scancode, int action, int mods) {
    EventManager::fire(EventKeyPress{key, scancode, action, mods});
}
