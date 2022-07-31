#include "window.hpp"

static void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

Window::Window()
    : camera(45.0f) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 16);

    window = glfwCreateWindow(1280, 720, APPLICATION_NAME.c_str(), NULL, NULL);
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

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

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
    camera.move(movement, dt * 20);

    double time = glfwGetTime();
    dt = time - last_time;
    last_time = time;

    return !glfwWindowShouldClose(window);
}
void Window::render() {
    renderer.render();

    glfwSwapBuffers(window);

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
}

void Window::callback_mouse_click(GLFWwindow* glfw_window, int button, int action, int mods) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_mouse_click(button, action, mods);
}
void Window::_callback_mouse_click(int button, int action, int mods) {
}

void Window::callback_key_press(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
    window->_callback_key_press(key, scancode, action, mods);
}
void Window::_callback_key_press(int key, int scancode, int action, int mods) {
}
