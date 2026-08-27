#include <Timer.hpp>

#include <GLFW/glfw3.h>

namespace blaze::lightEngine {

double Timer::getTime() {
	return glfwGetTime();
}

float Timer::getDelta() {
	double time = this->getTime();
	float delta = static_cast<float>(time - this->lastLoopTime);
	this->lastLoopTime = time;
	this->timeCount += delta;
	return delta;
}

void Timer::update() {
	if (this->timeCount < 1.0f) return;

	this->fps = this->fpsCount.exchange(0, std::memory_order_relaxed);
	this->ups = this->upsCount.exchange(0, std::memory_order_relaxed);

	this->timeCount -= 1.0f;
}

} // namespace blaze::lightEngine
