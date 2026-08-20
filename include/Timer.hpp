#pragma once

namespace blaze::lightEngine {

class Timer {
public:
	Timer();
	double getTime();
	float getDelta();
	void updateFPS();
	void updateUPS();
	void update();
	int getFPS();
	int getUPS();
	double getLastLoopTime();

private:
	double lastLoopTime = 0.0;
	float timeCount = 0.0f;
	int fpsCount = 0;
	int upsCount = 0;
	int fps = 0;
	int ups = 0;
};

} // namespace blaze::lightEngine
