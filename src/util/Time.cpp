#include <util/Time.hpp>

namespace blaze::lightEngine {

Time::Time(long microseconds): _microseconds(microseconds) {}

Time::~Time() {}

Time Time::seconds(float seconds) {
    return Time((long) (seconds * 1000000.0f));
}

Time Time::milliseconds(long milliseconds) {
    return Time(milliseconds * 1000L);
}

Time Time::microseconds(long microseconds) {
    return Time(microseconds);
}

long Time::asMilliseconds() {
    return this->_microseconds / 1000;
}

long Time::asMicroseconds() {
    return this->_microseconds;
}

Time Time::operator+(Time other) {
    return Time(this->_microseconds + other._microseconds);
}

Time Time::operator-(Time other) {
    return Time(this->_microseconds - other._microseconds);
}

std::string Time::toString() {
    long totalMicroseconds = this->_microseconds;
    long seconds = totalMicroseconds / 1000000;
    long milliseconds = (totalMicroseconds % 1000000) / 1000;
    long microseconds = totalMicroseconds % 1000;

    return std::to_string(seconds) + "s " + std::to_string(milliseconds) + "ms " + std::to_string(microseconds) + "us";
}

} // namespace blaze::lightEngine