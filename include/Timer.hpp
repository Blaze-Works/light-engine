#pragma once

#include <atomic>

namespace blaze::lightEngine {

class Timer {
private:
	double lastLoopTime;
	float timeCount = 0.0f;

	std::atomic<int> fpsCount{0};
	std::atomic<int> upsCount{0};
	std::atomic<int> fps{0};
	std::atomic<int> ups{0};

public:
	Timer() : lastLoopTime(getTime()) {}
	double getTime();
	float getDelta();
	void updateFPS() { this->fpsCount.fetch_add(1, std::memory_order_relaxed); }
	void updateUPS() { this->upsCount.fetch_add(1, std::memory_order_relaxed); }
	void update();
	int getFPS() { return this->fps.load(std::memory_order_relaxed); }
	int getUPS() { return this->ups.load(std::memory_order_relaxed); }
	double getLastLoopTime() { return this->lastLoopTime; }
};

} // namespace blaze::lightEngine
