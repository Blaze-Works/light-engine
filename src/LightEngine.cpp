#include <LightEngine.hpp>
#include <gl/ShaderPrograms.hpp>
#include <gl/ShaderLoader.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <texture/TextureUtil.hpp>
#include <util/Logger.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace blaze::lightEngine {

LightEngine* LightEngine::instance = nullptr;
static std::vector<int> fpsHistory;

LightEngine::LightEngine(RunArgs runArgs):
    runArgs(std::move(runArgs)),
    window(this, this->runArgs.windowSettings, "Untitled"),
    timer(),
    runDirectory(runArgs.directories.runDir),
    _is64Bit(checkIs64Bit()),
    running(true),
    windowFocused(false),
    shouldStop(false),
    isFullscreen(runArgs.windowSettings.fullscreen)
{
    instance = this;

    this->drawContext = std::make_shared<DrawContext>(&this->window);
    this->framebuffer = std::make_shared<WindowFramebuffer>(this->window.getFramebufferWidth(), this->window.getFramebufferHeight());

    this->window.setFramerateLimit(this->getMaxFPS());
    this->window.setPhase("Startup");
    this->window.logOnGlError();
    this->window.setPhase("Post startup");
    PathUtil::setResourceDir(this->runArgs.directories.resourceDir);
}

void LightEngine::initialize() {
    this->setShaderSource(PathUtil::resolveResource(std::string("./shaders")));
    try {
        ShaderPrograms::init();
    } catch (std::runtime_error& ex) {
        LOG_ERROR(std::string("Failed to initialize: ") + ex.what());
        exit(-1);
    }

    this->drawContext->init();
    this->framebuffer->initFbo(this->window.getFramebufferWidth(), this->window.getFramebufferHeight());
    this->window.onMouseDown(&LightEngine::onMouseDown);
    this->window.onMouseUp(&LightEngine::onMouseUp);
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

std::shared_ptr<WindowFramebuffer> LightEngine::getFramebuffer() {
    return this->framebuffer;
}

std::shared_ptr<DrawContext> LightEngine::getDrawContext() {
    return this->drawContext;
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
    this->framebuffer->resize(this->window.getFramebufferWidth(), this->window.getFramebufferHeight());

    for (auto& screen : this->screenStack) {
        screen->resize(framebuffer->getWidth(), framebuffer->getHeight());
    }
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

Window& LightEngine::getWindow() {
    return this->window;
}

const GuiNavigationType LightEngine::getNavigationType() {
    return GuiNavigationType::NONE;
}

void LightEngine::stop() {
    LOG_INFO("Stopping LightEngine...");
    if (!this->screenStack.empty()) {
        this->screenStack.back()->removed();
        this->screenStack.pop_back();
    }
    this->running = false;
    this->close();
}

void LightEngine::close() {
    try {
        this->framebuffer->terminate();
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
            Mouse::update(this->window.getHandle());

            this->window.swapBuffers();
            this->window.pollEvents();
            if (!this->window.isVsyncEnabled()) this->sync(this->targetFPS);
            if (this->shouldStop) this->stop();

        }
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("Thrown Exception: ") + ex.what());
    }
}

void LightEngine::render(float delta) {
    if (!this->framebuffer->isValid()) return;
    if (this->framebuffer->getWidth() < 1 || this->framebuffer->getHeight() < 1) return;

    this->window.setPhase("Render");
    this->drawContext->setProjection(this->framebuffer->getWidth(), this->framebuffer->getHeight());

    try {
        this->framebuffer->beginWrite(true);
        for (auto& cb : this->renderCallbacks) cb.second(*this->drawContext.get(), this->mouseX, this->mouseY, delta);
        for (auto& screen : this->screenStack) screen->render(*this->drawContext.get(), this->mouseX, this->mouseY, delta);
        if (this->runArgs.useDebug) this->drawDebug(delta);
        this->drawContext->flushTextureBatch();
        this->framebuffer->endWrite();
    } catch (const std::exception& ex) {
        LOG_ERROR(std::string("Framebuffer render failed") + ex.what());
        return;
    }

    this->framebuffer->drawFramebufferToScreen();
    this->drawContext->setProjection(this->window.getWidth(), this->window.getHeight());
    frames++;
    std::this_thread::yield();
    this->window.setPhase("Post render");
}

void LightEngine::drawDebug(float delta) {
    int fps = this->timer.getFPS();
    int ups = this->timer.getUPS();
    int time = static_cast<int>(this->timer.getTime());

    std::string posStr = "X: " + std::to_string(static_cast<int>(mouseX)) + " Y: " + std::to_string(static_cast<int>(mouseY));
    float textWidth = this->drawContext->getDebugTextWidth(posStr);
    float textHeight = this->drawContext->getDebugTextHeight(posStr);
    int w = this->window.getWidth();
    int h = this->window.getHeight();

    int _x;
    int _y;

    if (fpsHistory.empty() || fpsHistory.back() != fps) {
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > 500) {
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

    this->drawContext->setLayer(10);
    this->drawContext->setTransparent(true);
    this->drawContext->drawDebugText(5, 5,
        std::string("Context: ") + this->window.getContext() +
        std::string("\nFPS: ") + std::to_string(fps) + std::string(" | UPS: ") + std::to_string(ups) +
        std::string("\nMax FPS: ") + std::to_string(maxFPS) + std::string(" | Avg FPS: ") + std::to_string(avgFPS) + std::string(" | Min FPS: ") + std::to_string(minFPS) +
        std::string("\nFrames Count: ") + std::to_string(frames) + std::string("\nRun Time: ") + std::to_string(time) + std::string(" | Delta: ") + std::to_string(delta)
    );

    _x = MathUtils::clamp((int) this->mouseX, 0, (int) (w - textWidth + 6));
    _y = MathUtils::clamp((int) this->mouseY, 0, (int) (h - textHeight + 6));

    if (this->isCursorEntered()) {
        this->drawContext->drawQuad(_x + 6, _y + 6, textWidth + 4, textHeight + 4, 0x55000000);
        this->drawContext->drawDebugText(_x + 8, _y + 8, posStr);
    }
    this->drawContext->setTransparent(false);
    this->drawContext->setLayer(0);
}

int LightEngine::addRenderCallback(RenderCallback cb) {
    this->renderCallbacks[this->callbacks++] = std::move(cb);
    return this->callbacks;
}

void LightEngine::setScreen(std::unique_ptr<Screen> screen) {
    while (!this->screenStack.empty()) {
        this->screenStack.back()->removed();
        this->screenStack.pop_back();
    }

    if (screen) this->pushScreen(std::move(screen));
}

void LightEngine::pushScreen(std::unique_ptr<Screen> screen) {
    if (!screen) return;

    if (!this->screenStack.empty()) {
        this->screenStack.back()->blur();
    }

    Screen* raw = screen.get();
    this->screenStack.push_back(std::move(screen));
    raw->init(*this, this->window.getWidth(), this->window.getHeight());
    raw->onDisplayed();
}

void LightEngine::popScreen() {
    if (this->screenStack.empty()) return;

    this->screenStack.back()->removed();
    this->screenStack.pop_back();

    if (!this->screenStack.empty()) {
        Screen* top = this->screenStack.back().get();
        top->resize(this->window.getWidth(), this->window.getHeight());
        top->onDisplayed();
    }
}

Screen* LightEngine::getScreen() const {
    if (this->screenStack.empty()) return nullptr;
    return this->screenStack.back().get();
}

size_t LightEngine::getScreenStackSize() const {
    return this->screenStack.size();
}

void LightEngine::onMouseDown(int button, double xpos, double ypos) {
    LightEngine& engine = LightEngine::getInstance();
    engine.mouseX = xpos;
    engine.mouseY = ypos;
    if (!engine.screenStack.empty()) {
        engine.screenStack.back()->mouseClicked(xpos, ypos, button);
    }
}

void LightEngine::onMouseUp(int button, double xpos, double ypos) {
    LightEngine& engine = LightEngine::getInstance();
    engine.mouseX = xpos;
    engine.mouseY = ypos;
    if (!engine.screenStack.empty()) {
        engine.screenStack.back()->mouseReleased(xpos, ypos, button);
    }
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

