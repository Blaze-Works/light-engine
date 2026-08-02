#include <gl/ShaderPrograms.hpp>
#include <LightEngine.hpp>
#include <gl/ShaderLoader.hpp>
#include <input/Keyboard.hpp>
#include <texture/TextureUtil.hpp>
#include <util/Logger.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <exception>
#include <stdexcept>
#include <string>
#include <thread>

namespace blaze::lightEngine {

LightEngine* LightEngine::instance = nullptr;
static std::vector<int> fpsHistory;

LightEngine::LightEngine(RunArgs runArgs):
    runArgs(std::move(runArgs)),
    window(this, this->runArgs.windowSettings, "Untitled"),
    timer(),
    drawContext(&this->window),
    runDirectory(runArgs.directories.runDir),
    framebuffer(this->window.getFramebufferWidth(), this->window.getFramebufferHeight()),
    _is64Bit(checkIs64Bit()),
    running(true),
    windowFocused(false),
    shouldStop(false),
    isFullscreen(runArgs.windowSettings.fullscreen)
{
    instance = this;
    this->window.setFramerateLimit(this->getMaxFPS());
    this->window.setPhase("Startup");
    this->window.logOnGlError();
    this->window.setPhase("Post startup");
    PathUtil::setResourceDir(this->runArgs.directories.resourceDir);
    this->setShaderSource(PathUtil::resolveResource(std::string("./shaders")));
}

void LightEngine::initialize() {
    try {
        ShaderPrograms::init();
        this->drawContext.init();
    } catch (std::runtime_error& ex) {
        LOG_ERROR(std::string("Failed to initialize: ") + ex.what());
        exit(-1);
    }
    this->onWindowFocusChanged(true);
}

void LightEngine::sync(int fps) {
    double lastLoopTime = this->timer.getLastLoopTime();
    double now = this->timer.getTime();
    float targetTime = 1.0f / fps;
    double timeRemaining = targetTime - (now - lastLoopTime);

    while (timeRemaining > 0.002) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        now = this->timer.getTime();
        timeRemaining = targetTime - (now - lastLoopTime);
    }

    while (timeRemaining > 0.0) {
        std::this_thread::yield();
        now = this->timer.getTime();
        timeRemaining = targetTime - (now - lastLoopTime);
    }
}

bool LightEngine::getFullscreen() {
    return this->isFullscreen;
}

void LightEngine::setFullscreen(bool fullscreen) {
    this->isFullscreen = fullscreen;
}

bool LightEngine::getEnableVsync() {
    return this->window.isVsyncEnabled();
}

int LightEngine::getMaxFPS() {
    return this->targetFPS;
}

int LightEngine::getMaxUPS() {
    return this->targetUPS;
}

void LightEngine::setMaxFPS(int fps) {
    this->targetFPS = fps;
}

void LightEngine::setMaxUPS(int ups) {
    this->targetUPS = ups;
}

std::string LightEngine::getWindowTitle() {
    return this->window.getTitle();
}

void LightEngine::setWindowTitle(std::string title) {
    this->window.setTitle(std::move(title));
}

WindowFramebuffer LightEngine::getFramebuffer() {
    return this->framebuffer;
}

bool LightEngine::forcesUnicodeFont() const {
    return false;
}

int LightEngine::getFramerateLimit() {
    return 200;
}

void LightEngine::onResolutionChanged() {
    int i = this->window.calculateScaleFactor(2, this->forcesUnicodeFont());
    this->window.setScaleFactor(i);
    WindowFramebuffer framebuffer = this->getFramebuffer();
    framebuffer.resize(this->window.getFramebufferWidth(), this->window.getFramebufferHeight());
}

bool LightEngine::is64Bit() {
    return this->_is64Bit;
}

std::thread::id LightEngine::getThread() {
    return std::this_thread::get_id();
}

bool LightEngine::isWindowFocused() {
    return this->windowFocused;
}

void LightEngine::onWindowFocusChanged(bool focused) {
    this->windowFocused = focused;
}

bool LightEngine::isCursorEntered() {
    return this->cursorEntered;
}

void LightEngine::onCursorEnterChanged() {
    this->cursorEntered = this->window.getCursorEnterState();
}

void LightEngine::onCursorPosChanged() {
    this->mouseX = this->window.getMouseX();
    this->mouseY = this->window.getMouseY();
}

const Window& LightEngine::getWindow() const {
    return this->window;
}

void LightEngine::stop() {
    LOG_INFO("Stopping LightEngine...");
    this->running = false;
    this->close();
}

void LightEngine::close() {
    try {
        this->framebuffer.terminate();
    } catch (std::exception ex) {
        LOG_ERROR(std::string("Failed to terminate framebuffer: ") + ex.what());
        throw ex;
    }

    this->window.close();
}

void LightEngine::run() {
    float delta;
    float accumulator = 0.0f;
    float interval = 1.0f;
    float alpha;

    this->onResolutionChanged();

    try {
        interval = 1.0f / static_cast<float>(this->targetUPS > 0 ? this->targetUPS : 1);

        while (this->running && !this->window.shouldClose()) {
            delta = this->timer.getDelta();
            accumulator += delta;

            if (delta > 0.25f) delta = 0.25f;

            while (accumulator >= interval) {
                this->timer.updateUPS();
                accumulator -= interval;
            }

            alpha = accumulator / interval;

            try {
                this->render(alpha);
            } catch (const std::exception& ex) {
                throw std::runtime_error(std::string("Render Exception: ") + ex.what());
            }

            this->timer.updateFPS();
            this->timer.update();

            Keyboard::update(this->window.getHandle());

            this->window.swapBuffers();
            this->window.pollEvents();
            if (!this->window.isVsyncEnabled()) this->sync(this->targetFPS);
            if (this->shouldStop) this->stop();

        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("Thrown Exception: ") + ex.what());
    }
}

void LightEngine::render(float alpha) {
    if (!this->framebuffer.isValid()) return;
    if (this->framebuffer.getWidth() < 1 || this->framebuffer.getHeight() < 1) return;

    this->window.setPhase("Render");
    this->drawContext.setProjection(this->framebuffer.getWidth(), this->framebuffer.getHeight());

    try {
        this->framebuffer.beginWrite(true);
        this->drawDebug();
        for (auto& cb : this->renderCallbacks) cb.second(this->drawContext, this->mouseX, this->mouseY, this->timer.getDelta());
        this->framebuffer.endWrite();
    } catch (const std::exception& ex) {
        LOG_ERROR(std::string("Framebuffer render failed") + ex.what());
        return;
    }

    this->framebuffer.drawFramebufferToScreen();
    this->drawContext.setProjection(this->window.getWidth(), this->window.getHeight());
    frames++;
    std::this_thread::yield();
    this->window.setPhase("Post render");
}

void LightEngine::drawDebug() {
    int fps = this->timer.getFPS();
    int ups = this->timer.getUPS();
    int time = static_cast<int>(this->timer.getTime());

    std::string posStr = "X: " + std::to_string(static_cast<int>(mouseX)) + " Y: " + std::to_string(static_cast<int>(mouseY));
    float textWidth = this->drawContext.getDebugTextWidth(posStr);
    float textHeight = this->drawContext.getDebugTextHeight(posStr);
    int w = this->window.getWidth();
    int h = this->window.getHeight();

    int _x;
    int _y;

    if (fpsHistory.empty() || fpsHistory.back() != fps) {
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > 100) {
            fpsHistory.erase(fpsHistory.begin());
        }

        int sum = 0;
        for (int val : fpsHistory) sum += val;
        avgFPS = sum / fpsHistory.size();
    }

    if (frames % 500 == 0 && fps > 0) {
        maxFPS = fps;
        minFPS = fps;
    } else if (fps > 0) {
        maxFPS = std::max(maxFPS, fps);
        minFPS = (minFPS == 0) ? fps : std::min(minFPS, fps);
    }

    this->drawContext.setLayer(10);
    this->drawContext.drawDebugText(5, 5,
        std::string("Context: ") + this->window.getContext() +
        std::string("\nFPS: ") + std::to_string(fps) + std::string(" | UPS: ") + std::to_string(ups) +
        std::string("\nMax FPS: ") + std::to_string(maxFPS) + std::string(" | Avg FPS: ") + std::to_string(avgFPS) + std::string(" | Min FPS: ") + std::to_string(minFPS) +
        std::string("\nFrames Count: ") + std::to_string(frames) + std::string("\nRun Time: ") + std::to_string(time)
    );

    _x = MathUtils::clamp((int) this->mouseX, 0, (int) (w - textWidth + 6));
    _y = MathUtils::clamp((int) this->mouseY, 0, (int) (h - textHeight + 6));

    if (this->isCursorEntered()) {
        this->drawContext.drawDebugText(_x + 8, _y + 8, posStr);
        this->drawContext.drawQuad(_x + 6, _y + 6, textWidth + 4, textHeight + 4, 0x55000000);
    }
    this->drawContext.setLayer(0);
}

int LightEngine::addRenderCallback(RenderCallback cb) {
    this->renderCallbacks[this->callbacks++] = std::move(cb);
    return this->callbacks;
}

void LightEngine::removeRenderCallback(int i) {
    this->renderCallbacks.erase(i);
}

void LightEngine::scheduleStop() {
    this->shouldStop = true;
}

bool LightEngine::isRunning() const noexcept {
    return this->running;
}

bool LightEngine::shouldRenderAsync() {
    return false;
}

double LightEngine::getTime() {
    return this->timer.getTime();
}

void LightEngine::setShaderSource(std::string path) {
    ShaderLoader::setShaderSource(path);
}

LightEngine& LightEngine::getInstance() noexcept {
    return *instance;
}

bool LightEngine::checkIs64Bit() {
    return sizeof(void*) == 8;
}

} // namespace blaze::lightEngine

