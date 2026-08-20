#include <gl/glad.h>
#include <string>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <util/Window.hpp>
#include <util/Logger.hpp>

#include <exception>

namespace blaze::lightEngine {

double Window::lastDrawTime = 0.0;

Window::Window(WindowEventHandler* eventHandler, WindowSettings settings, const std::string& title) {
	this->eventHandler = eventHandler;
	this->title = title;
	this->width = settings.width > 0 ? settings.width : 1;
	this->height = settings.height > 0 ? settings.height : 1;
	this->framebufferWidth = 0;
	this->framebufferHeight = 0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->phase = "";
	this->framerateLimit = 0;
	this->vsync = false;
	this->fullscreen = settings.fullscreen;
	this->currentFullscreen = settings.fullscreen;
	this->scaledWidth = this->width;
	this->scaledHeight = this->height;
	this->windowedX = 0;
	this->windowedY = 0;
	this->windowedWidth = this->width;
	this->windowedHeight = this->height;
	this->minimized = false;
	this->cursorEntered = false;

	throwOnGlError();
	setPhase("Pre startup");

	LOG_INFO(std::string(settings.describe()));

	if (!glfwInit()) {
		LOG_ERROR("Failed to initialize GLFW");
		exit(-1);
	}

	glfwSetErrorCallback(throwGlError);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	this->context = "3.3 Core";

	this->handle = glfwCreateWindow(this->width, this->height, title.c_str(), nullptr, nullptr);
	if (!this->handle) {
		LOG_ERROR("Failed to create GLFW window with 3.3 core profile, trying fallbacks...");

		// Fallback 1: try 3.3 without forcing core profile
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		this->context = "3.1";

		this->handle = glfwCreateWindow(this->width, this->height, title.c_str(), nullptr, nullptr);
	}

	if (!this->handle) {
		LOG_ERROR("Fallback: trying OpenGL 2.1 compatibility profile...");
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		this->context = "2.1";

		this->handle = glfwCreateWindow(this->width, this->height, title.c_str(), nullptr, nullptr);
	}

	if (!this->handle) {
		LOG_ERROR("Failed to create GLFW window after fallbacks");
		glfwTerminate();
		exit(-1);
	}

	glfwSetWindowUserPointer(this->handle, this);
	glfwMakeContextCurrent(this->handle);
	glfwShowWindow(this->handle);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		LOG_ERROR("Failed to initialize GLAD");
		exit(-1);
	}

	// Enable GL debug callbacks when available
	if (GLAD_GL_VERSION_4_3 || GLAD_GL_KHR_debug) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			(void)source; (void)type; (void)id; (void)length; (void)userParam;
			// Log all messages as errors for visibility
			LOG_ERROR(std::string("GL Debug: ") + message);
		}, nullptr);
	}

	glViewport(0, 0, this->width, this->height);
	this->updateFramebufferSize();
	this->setupCallbacks();
}

Window::~Window() {
	glfwDestroyWindow(handle);
	glfwTerminate();
}

void Window::setupCallbacks() {
	glfwSetFramebufferSizeCallback(this->handle, onFramebufferSizeChanged);
	glfwSetWindowPosCallback(this->handle, onWindowPosChanged);
	glfwSetWindowSizeCallback(this->handle, onWindowSizeChanged);
	glfwSetWindowFocusCallback(this->handle, onWindowFocusChanged);
	glfwSetCursorEnterCallback(this->handle, onCursorEnterChanged);
	glfwSetWindowIconifyCallback(this->handle, onMinimizeChanged);
	glfwSetMouseButtonCallback(this->handle, onMouseButtonListeners);
	glfwSetKeyCallback(this->handle, onKeyListeners);
	glfwSetCursorPosCallback(this->handle, onMouseMoveListeners);
	glfwSetScrollCallback(this->handle, onMouseWheelListeners);
}

void Window::setPhase(const std::string phase) {
	this->phase = phase;
}

void Window::setPos(int x, int y) {
	glfwSetWindowPos(this->handle, x, y);
}

void Window::updateFramebufferSize() {
	int is[1] = {};
	int js[1] = {};
	glfwGetFramebufferSize(this->handle, is, js);
	this->framebufferWidth = is[0] > 0 ? is[0] : 1;
	this->framebufferHeight = js[0] > 0 ? js[0] : 1;
}

void Window::setIconFromFile() {

}

void Window::setIconFromResource() {

}

void Window::setIcon() {

}

void Window::setFramerateLimit(int framerateLimit) {
	this->framerateLimit = framerateLimit;
}

std::string Window::getTitle() {
	return this->title;
}

std::string Window::getContext() {
	return this->context;
}

void Window::setTitle(const std::string& title) {
	this->title = title;
	glfwSetWindowTitle(this->handle, this->title.c_str());
}

GLFWwindow* Window::getHandle() const{
	return this->handle;
}

int Window::getFramebufferWidth() {
	return this->framebufferWidth;
}

int Window::getFramebufferHeight() {
	return this->framebufferHeight;
}

void Window::setFramebufferWidth(int framebufferWidth) {
	this->framebufferWidth = framebufferWidth;
}

void Window::setFramebufferHeight(int framebufferHeight) {
	this->framebufferHeight = framebufferHeight;
}

int Window::getWidth() {
	return this->width;
}

int Window::getHeight() {
	return this->height;
}

int Window::getScaledWidth() {
	return this->scaledWidth;
}

int Window::getScaledHeight() {
	return this->scaledHeight;
}

int Window::getX() {
	return this->x;
}

int Window::getY() {
	return this->y;
}

float Window::getAspectRatio() {
	return (float)this->width / (float) this->height;
}

bool Window::getCursorEnterState() {
	return this->cursorEntered;
}

double Window::getMouseX() {
	return this->mouseX;
}

double Window::getMouseY() {
	return this->mouseY;
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(this->handle);
}

void Window::limitDisplayFPS(int fps) {
	double d = lastDrawTime + 1.0 / (double)fps;
	double e;

	for (e = glfwGetTime(); e < d; e = glfwGetTime()) {
		glfwWaitEventsTimeout(d - e);
	}

	lastDrawTime = e;
}

void Window::pollEvents() {
	glfwPollEvents();
}

void Window::swapBuffers() {
	if (this->fullscreen != this->currentFullscreen) {
		this->currentFullscreen = this->fullscreen;
		this->updateFullScreen(this->vsync);
	}
	glfwSwapBuffers(this->handle);
}

void Window::updateFullScreen(bool vsync) {
	try {
		this->updateWindowRegion();
		this->setVsync(vsync);
		this->swapBuffers();
	} catch (std::exception e) {

	}
}

void Window::setVsync(bool vsync) {
	this->vsync = vsync;
	glfwSwapInterval((int) vsync);
}

bool Window::isVsyncEnabled() {
	return this->vsync;
}

void Window::updateWindowRegion() {
	bool bl = glfwGetWindowMonitor(this->handle) != 0L;
	if (this->fullscreen) {
		// Fullscreen mode not implemented.
	} else {
		this->x = this->windowedX;
		this->y = this->windowedY;
		this->width = this->windowedWidth;
		this->height = this->windowedHeight;
		glfwSetWindowMonitor(this->handle, 0L, this->x, this->y, this->width, this->height, -1);
	}
}

void Window::logOnGlError() {
	// No-op: placeholder for OpenGL error logging.
}

void Window::close() {
	if (this->handle) {
		glfwDestroyWindow(this->handle);
		glfwTerminate();
		this->handle = nullptr;
	}
}

void Window::throwOnGlError() {
	// No-op: placeholder before OpenGL initialization.
}

void Window::setScaleFactor(double scaleFactor) {
	int i = (int)((double)this->framebufferWidth / scaleFactor);
	this->scaledWidth = (double)this->framebufferWidth / scaleFactor > (double)i ? i + 1 : i;
	int j = (int)((double)this->framebufferHeight / scaleFactor);
	this->scaledHeight = (double)this->framebufferHeight / scaleFactor > (double)j ? j + 1 : j;
}

int Window::calculateScaleFactor(int guiScale, bool forceUnicodeFont) {
	int i;
	for (i = 1; i != guiScale && i < this->framebufferWidth && 
		i < this->framebufferHeight && this->framebufferWidth / (i * 1) >= 320 &&
		this->framebufferHeight / (i + 1) >= 240; ++i) {}

	if (forceUnicodeFont && i % 2 != 0) ++i;
	return i;
}

void Window::onFramebufferSizeChanged(GLFWwindow* window, int width, int height) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self && self->eventHandler) {
		self->framebufferWidth = width > 0 ? width : 1;
		self->framebufferHeight = height > 0 ? height : 1;
		self->eventHandler->onResolutionChanged();
	}
}

void Window::onWindowPosChanged(GLFWwindow* window, int x, int y) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->x = x;
		self->y = y;
	}
}

void Window::onWindowSizeChanged(GLFWwindow* window, int width, int height) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->width = width > 0 ? width : 1;
		self->height = height > 0 ? height : 1;
		self->updateFramebufferSize();
		self->eventHandler->onResolutionChanged();

		for (WindowResizefun l : self->resizeListeners) l(self->width, self->height);
	}
}

void Window::onWindowFocusChanged(GLFWwindow* window, int focused) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self && self->eventHandler) {
		self->eventHandler->onWindowFocusChanged(focused != 0);
	}
}

void Window::onCursorEnterChanged(GLFWwindow* window, int entered) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->cursorEntered = entered != 0;
		if (self->eventHandler) {
			self->eventHandler->onCursorEnterChanged();
		}
	}
}

void Window::onCursorPosChanged(GLFWwindow* window, double mouseX, double mouseY) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->mouseX = mouseX;
		self->mouseY = mouseY;
		if (self->eventHandler) {
			self->eventHandler->onCursorPosChanged();
		}

		for (WindowMouseMovefun l : self->mouseMoveListeners) l(mouseX, mouseY);
	}
}

void Window::onMinimizeChanged(GLFWwindow* window, int minimized) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->minimized = minimized != 0;
	}
}

void Window::onMouseButtonListeners(GLFWwindow* window, int button, int action, int mods) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self) {
		double xpos[1];
		double ypos[1];
		glfwGetCursorPos(window, xpos, ypos);

		if (action == GLFW_PRESS) {
			for (WindowMouseDownfun l : self->mouseDownListeners) l(button, xpos[0], ypos[0]);
		} else if (action == GLFW_RELEASE) {
			for (WindowMouseUpfun l : self->mouseUpListeners) l(button, xpos[0], ypos[0]);
		}
	}

	Mouse::mouseButtonCallback(window, button, action, mods);
}

void Window::onKeyListeners(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self) {
		if (action == GLFW_PRESS) {
			for (WindowKeyDownfun l : self->keyDownListeners) l(key);
		} else if (action == GLFW_RELEASE) {
			for (WindowKeyUpfun l : self->keyUpListeners) l(key);
		}
	}

	Keyboard::keyCallback(window, key, scancode, action, mods);
}

void Window::onMouseMoveListeners(GLFWwindow* window, double xpos, double ypos) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (self) {
		self->mouseX = xpos;
		self->mouseY = ypos;
		if (self->eventHandler) {
			self->eventHandler->onCursorPosChanged();
		}
	}
}

void Window::onMouseWheelListeners(GLFWwindow* window, double xoffset, double yoffset) {
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (self) {
		for (WindowMouseScrollfun l : self->mouseScrollListeners) l(xoffset, yoffset);
	}
}

void Window::clear() const {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::throwGlError(int error, const char* description) {
	LOG_ERROR(std::string("GLFW Error (") + std::to_string(error) + ") " + description);
}

void Window::onKeyDown(WindowKeyDownfun listener) {
	this->keyDownListeners.push_back(listener);
}

void Window::onKeyUp(WindowKeyUpfun listener) {
	this->keyUpListeners.push_back(listener);
}

void Window::onMouseDown(WindowMouseDownfun listener) {
	this->mouseDownListeners.push_back(listener);
}

void Window::onMouseMove(WindowMouseMovefun listener) {
	this->mouseMoveListeners.push_back(listener);
}

void Window::onMouseUp(WindowMouseUpfun listener) {
	this->mouseUpListeners.push_back(listener);
}

void Window::onMouseScroll(WindowMouseScrollfun listener) {
	this->mouseScrollListeners.push_back(listener);
}

void Window::onResize(WindowResizefun listener) {
	this->resizeListeners.push_back(listener);
}

void Window::offKeyDown(WindowKeyDownfun listener) {
	auto it = std::find_if(this->keyDownListeners.begin(), this->keyDownListeners.end(), [listener](const std::function<void(int)>& fn) {
		// auto* const* target_ptr = fn.target<WindowKeyDownfun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowKeyDownfun>() == listener.target<WindowKeyDownfun>();
	});

	if (it != this->keyDownListeners.end()) {
		this->keyDownListeners.erase(it);
	}
}

void Window::offKeyUp(WindowKeyUpfun listener) {
	auto it = std::find_if(this->keyUpListeners.begin(), this->keyUpListeners.end(), [listener](const std::function<void(int)>& fn) {
		// auto* const* target_ptr = fn.target<WindowKeyUpfun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowKeyUpfun>() == listener.target<WindowKeyUpfun>();
	});

	if (it != this->keyUpListeners.end()) {
		this->keyUpListeners.erase(it);
	}
}

void Window::offMouseDown(WindowMouseDownfun listener) {
	auto it = std::find_if(this->mouseDownListeners.begin(), this->mouseDownListeners.end(), [listener](const std::function<void(int, double, double)>& fn) {
		// auto* const* target_ptr = fn.target<WindowMouseDownfun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowMouseDownfun>() == listener.target<WindowMouseDownfun>();
	});

	if (it != this->mouseDownListeners.end()) {
		this->mouseDownListeners.erase(it);
	}
}

void Window::offMouseMove(WindowMouseMovefun listener) {
	auto it = std::find_if(this->mouseMoveListeners.begin(), this->mouseMoveListeners.end(), [listener](const std::function<void(double, double)>& fn) {
		// auto* const* target_ptr = fn.target<WindowMouseMovefun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowMouseMovefun>() == listener.target<WindowMouseMovefun>();
	});

	if (it != this->mouseMoveListeners.end()) {
		this->mouseMoveListeners.erase(it);
	}
}

void Window::offMouseUp(WindowMouseUpfun listener) {
	auto it = std::find_if(this->mouseUpListeners.begin(), this->mouseUpListeners.end(), [listener](const std::function<void(int, double, double)>& fn) {
		//mauto* const* target_ptr = fn.target<WindowMouseUpfun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowMouseUpfun>() == listener.target<WindowMouseUpfun>();
	});

	if (it != this->mouseUpListeners.end()) {
		this->mouseUpListeners.erase(it);
	}
}

void Window::offMouseScroll(WindowMouseScrollfun listener) {
	auto it = std::find_if(this->mouseScrollListeners.begin(), this->mouseScrollListeners.end(),[listener](const std::function<void(double, double)>& fn) {
		// auto* const* target_ptr = fn.target<WindowMouseScrollfun>();
		// return target_ptr && (*target_ptr == listener);
		return fn.target<WindowMouseScrollfun>() == listener.target<WindowMouseScrollfun>();
	});

	if (it != this->mouseScrollListeners.end()) {
		this->mouseScrollListeners.erase(it);
	}
}

void Window::offResize(WindowResizefun listener) {
	auto it = std::find_if(this->resizeListeners.begin(), this->resizeListeners.end(), [listener](const WindowResizefun& fn) {
		return listener.target<WindowResizefun>() == fn.target<WindowResizefun>();
	});

	if (it != this->resizeListeners.end()) {
		this->resizeListeners.erase(it);
	}
}

} // namespace blaze::lightEngine
