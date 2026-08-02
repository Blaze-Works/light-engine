#include <Timer.hpp>

#include <GLFW/glfw3.h>

namespace blaze::lightEngine {

Timer::Timer(): lastLoopTime(getTime()) {}

double Timer::getTime() {
    return glfwGetTime();
}

float Timer::getDelta() {
    double time = this->getTime();
    float delta = (float) (time - this->lastLoopTime);
    this->lastLoopTime = time;
    this->timeCount += delta;
    return delta;
}

void Timer::updateFPS() {
    this->fpsCount++;
}

void Timer::updateUPS() {
    this->upsCount++;
}

void Timer::update() {
    if (this->timeCount < 1.0f) return;

    this->fps = this->fpsCount;
    this->fpsCount = 0;
    this->ups = this->upsCount;
    this->upsCount = 0;

    this->timeCount -= 1;
}

int Timer::getFPS() {
    return this->fps;
}

int Timer::getUPS() {
    return this->ups;
}

double Timer::getLastLoopTime() {
    return this->lastLoopTime;
}

} // namespace blaze::lightEngine
