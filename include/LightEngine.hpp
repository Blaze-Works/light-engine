#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <thread>

#include "RunArgs.hpp"
#include "Timer.hpp"
#include "WindowEventHandler.hpp"
#include "gl/WindowFramebuffer.hpp"
#include "gui/DrawContext.hpp"
#include "util/Window.hpp"

namespace blaze::lightEngine {

using RenderCallback = std::function<void(DrawContext&, int, int, float)>;

class LightEngine: public WindowEventHandler {
public:
    explicit LightEngine(RunArgs runArgs);

    const std::string runDirectory;

    void initialize();
    void sync(int fps);
    bool getFullscreen();
    void setFullscreen(bool fullscreen);
    bool getEnableVsync();
    int getMaxFPS();
    int getMaxUPS();
    void setMaxFPS(int fps);
    void setMaxUPS(int ups);
    std::string getWindowTitle();
    void setWindowTitle(std::string title);
    WindowFramebuffer getFramebuffer();
    DrawContext getDrawContext();
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
    void stop();
    void close();
    void run();
    void render(float alpha);
    int addRenderCallback(RenderCallback cb);
    void removeRenderCallback(int i);
    void scheduleStop();
    [[nodiscard]] bool isRunning() const noexcept;
    bool shouldRenderAsync();
    double getTime();
    void setShaderSource(std::string path);

    static LightEngine& getInstance() noexcept;

private:
    void tick();
    void renderFrame();
    void drawDebug();

    RunArgs runArgs;
    Window window;
    WindowFramebuffer framebuffer;
    DrawContext drawContext;
    const bool _is64Bit;
    bool running;
    bool checkIs64Bit();
    bool windowFocused;
    bool shouldStop;
    bool isFullscreen;
    bool cursorEntered;
    double mouseX = 0.0;
    double mouseY = 0.0;
    int targetFPS = 60;
    int targetUPS = 20;

    int callbacks = 0;
    std::map<int, RenderCallback> renderCallbacks;

    int maxFPS = 0;
    int avgFPS = 0;
    int minFPS = 0;
    int frames = 0;
    static LightEngine* instance;

protected:
    Timer timer;
};

} // namespace blaze::lightEngine

