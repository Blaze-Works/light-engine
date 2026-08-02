#include <util/Clock.hpp>

#include <chrono>

namespace blaze::lightEngine {

Clock::Clock() {
    this->reset();
}

void Clock::reset() {
    this->startTime = this->getTime();
}

Time Clock::getElapsedTime() {
    long currentTime = this->getTime();
    long micros = (currentTime - this->startTime);
    return Time::microseconds(micros);
}

Time Clock::restart() {
    Time elasped = this->getElapsedTime();
    this->reset();
    return elasped;
}

long Clock::getTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    
    return microseconds;
}

} // namespace blaze::lightEngine