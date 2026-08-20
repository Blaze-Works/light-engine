#pragma once

#include <WindowEventHandler.hpp>
#include <WindowSettings.hpp>

#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <vector>

namespace blaze::lightEngine {

using WindowKeyDownfun     = std::function<void(int keycode)>;
using WindowKeyUpfun       = std::function<void(int keycode)>;
using WindowMouseScrollfun = std::function<void(double xoffset, double yoffset)>;
using WindowMouseDownfun   = std::function<void(int button, double xpos, double ypos)>;
using WindowMouseMovefun   = std::function<void(double xpos, double ypos)>;
using WindowMouseUpfun     = std::function<void(int, double xpos, double ypos)>;
using WindowResizefun      = std::function<void(int width, int height)>;
	
class Window {

public:
	Window(WindowEventHandler* eventHandler, WindowSettings settings, const std::string& title);
	~Window();

	void setPhase(const std::string phase);
	void setPos(int x, int y);
	void setIconFromFile();
	void setIconFromResource();
	void setIcon();
	void logGLError(int error, const char** description);
	void logOnGlError();
	void close();
	void setFramerateLimit(int framerateLimit);
	std::string getTitle();
	std::string getContext();
	void setTitle(const std::string& title);
	GLFWwindow* getHandle() const;
	int getFramebufferWidth();
	int getFramebufferHeight();
	void setFramebufferWidth(int framebufferWidth);
	void setFramebufferHeight(int framebufferHeight);
	int getWidth();
	int getHeight();
	int getScaledWidth();
	int getScaledHeight();
	int getX();
	int getY();
	float getAspectRatio();
	bool getCursorEnterState();
	double getMouseX();
	double getMouseY();
	bool shouldClose() const;
	void limitDisplayFPS(int fps);
	void pollEvents();
	void swapBuffers();
	void updateFullscreen(bool vsync);
	bool isVsyncEnabled();
	void setScaleFactor(double scaleFactor);
	int calculateScaleFactor(int guiScale, bool forceUnicode);

	void onKeyDown(WindowKeyDownfun callback);
	void onKeyUp(WindowKeyUpfun callback);
	void onMouseDown(WindowMouseDownfun callback);
	void onMouseMove(WindowMouseMovefun callback);
	void onMouseUp(WindowMouseUpfun callback);
	void onMouseScroll(WindowMouseScrollfun callback);
	void onResize(WindowResizefun callback);
	void offKeyDown(WindowKeyDownfun callback);
	void offKeyUp(WindowKeyUpfun callback);
	void offMouseDown(WindowMouseDownfun callback);
	void offMouseMove(WindowMouseMovefun callback);
	void offMouseUp(WindowMouseUpfun callback);
	void offMouseScroll(WindowMouseScrollfun callback);
	void offResize(WindowResizefun callback);

	double getScaleFactor();
	void clear() const;

private:
	GLFWwindow* handle;
	WindowEventHandler* eventHandler;
	std::string title;
	std::string context;
	int x;
	int y;
	int width;
	int height;
	int framebufferWidth;
	int framebufferHeight;
	double mouseX;
	double mouseY;
	std::string phase;
	int framerateLimit;
	bool vsync;
	bool fullscreen;
	bool currentFullscreen;
	int scaledWidth;
	int scaledHeight;
	int windowedX;
	int windowedY;
	int windowedWidth;
	int windowedHeight;
	bool minimized;
	bool cursorEntered;

	void setupCallbacks();
	void throwOnGlError();
	void updateFramebufferSize();
	void updateFullScreen(bool vsync);
	void setVsync(bool vsync);
	void updateWindowRegion();

	std::vector<std::function<void(int)>> keyDownListeners;
	std::vector<std::function<void(int)>> keyUpListeners;
	std::vector<std::function<void(double, double)>> mouseScrollListeners;
	std::vector<std::function<void(int, double, double)>> mouseDownListeners;
	std::vector<std::function<void(double, double)>> mouseMoveListeners;
	std::vector<std::function<void(int, double, double)>> mouseUpListeners;
	std::vector<WindowResizefun> resizeListeners;

	static void onFramebufferSizeChanged(GLFWwindow* handle, int width, int height);
	static void onWindowPosChanged(GLFWwindow* handle, int x, int y);
	static void onWindowSizeChanged(GLFWwindow* handle, int widtg, int height);
	static void onWindowFocusChanged(GLFWwindow* handle, int focused);
	static void onCursorEnterChanged(GLFWwindow* handle, int entered);
	static void onCursorPosChanged(GLFWwindow* handle, double mouseX, double mouseY);
	static void onMinimizeChanged(GLFWwindow* handle, int minimized);
	static void onMouseButtonListeners(GLFWwindow* handle, int button, int action, int mods);
	static void onKeyListeners(GLFWwindow* handle, int key, int scancose, int action, int mods);
	static void onMouseMoveListeners(GLFWwindow* handle, double xpos, double ypos);
	static void onMouseWheelListeners(GLFWwindow* handle, double xoffset, double yoffset);

	static double lastDrawTime;
	static void throwGlError(int error, const char* description);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

} // namespace blaze::lightEngine
