#pragma once

#include <string>

namespace blaze::lightEngine {

class Time {
public:
    Time(long microseconds);
    ~Time();

    static Time seconds(float seconds);
    static Time milliseconds(long milliseconds);
    static Time microseconds(long microseconds);
    float asSeconds();
    long asMilliseconds();
    long asMicroseconds();
    Time operator+(Time other);
    Time operator-(Time other);
    std::string toString();

private:
    // static const Time ZERO = Time(0);
    const long _microseconds;

};

}