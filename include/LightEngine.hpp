#pragma once

#include <cstddef>
#include <string>
#include <thread>

#include <RunArgs.hpp>
#include <Timer.hpp>
#include <WindowEventHandler.hpp>
#include <gl/WindowFramebuffer.hpp>
#include <gui/navigation/GuiNavigationType.hpp>
#include <gui/screen/Screen.hpp>
#include <render/DrawContext.hpp>
#include <util/Window.hpp>

namespace blaze::lightEngine {

using RenderCallback = std::function<void(DrawContext&, int, int, float)>;
using UpdateCallback = std::function<void(float)>;

class LightEngine: public WindowEventHandler {

public:
	explicit LightEngine(RunArgs runArgs = RunArgs());

	const std::string runDirectory;

	void initialize();
	void sync(int targetRate, double& lastTime, Timer& timer);
	bool getFullscreen();
	void setFullscreen(bool fullscreen);
	bool getEnableVsync();
	int getMaxFPS();
	int getMaxUPS();
	void setMaxFPS(int fps);
	void setMaxUPS(int ups);
	std::string getWindowTitle();
	void setWindowTitle(std::string title);
	std::shared_ptr<WindowFramebuffer> getFramebuffer();
	std::shared_ptr<DrawContext> getDrawContext();
	bool forcesUnicodeFont() const;
	int getFramerateLimit();
	void onResolutionChanged();
	bool is64Bit();
	std::thread::id getThread();
	bool isWindowFocused();
	void onWindowFocusChanged(bool focused);
	bool isCursorEntered();
	void onCursorEnterChanged();
	void onCursorPosChanged();
	const Window& getWindow() const;
	Window& getWindow();
	const GuiNavigationType getNavigationType();
	static void onMouseDown(int button, double xpos, double ypos);
	static void onMouseUp(int button, double xpos, double ypos);
	void stop();
	void close();
	void run();
	int addRenderCallback(RenderCallback cb);
	void removeRenderCallback(int i);
	int addUpdateCallback(UpdateCallback cb);
	void removeTickCallback(int i);
	void scheduleStop();
	void setScreen(std::unique_ptr<Screen> screen);
	void pushScreen(std::unique_ptr<Screen> screen);
	void popScreen();
	Screen* getScreen() const;
	size_t getScreenStackSize() const;
	[[nodiscard]] bool isRunning() const noexcept;
	void shouldRenderAsync(bool bl);
	double getTime();
	void setShaderSource(std::string path);

	static LightEngine& getInstance() noexcept;

private:
	void update(float delta);
	void render(float delta);
	void runUpdateLoop(Timer& timer);
	void runSingleThread();
	void runMultiThread();
	void drawDebug(float delta);

	RunArgs runArgs;
	Window window;
	std::shared_ptr<WindowFramebuffer> framebuffer;
	std::shared_ptr<DrawContext> drawContext;
	std::vector<std::unique_ptr<Screen>> screenStack;

	const bool _is64Bit;
	bool running;
	bool checkIs64Bit();
	bool windowFocused;
	bool shouldStop;
	bool isFullscreen;
	bool cursorEntered;
	bool renderAsync = false;
	double mouseX = 0.0;
	double mouseY = 0.0;
	int targetFPS = 60;
	int targetUPS = 20;

	int renderCallback = 0;
	int updateCallback = 0;
	std::map<int, RenderCallback> renderCallbacks;
	std::map<int, UpdateCallback> updateCallbacks;

	int maxFPS = 0;
	int avgFPS = 0;
	int minFPS = 0;
	int frames = 0;
	static LightEngine* instance;

protected:
	Timer timer;
};

} // namespace blaze::lightEngine

