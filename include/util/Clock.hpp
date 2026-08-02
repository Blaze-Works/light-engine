#pragma once

#include "Time.hpp"

namespace blaze::lightEngine {

class Clock {
public:
    Clock();
    ~Clock();

    void reset();
    Time getElapsedTime();
    Time restart();
    long getTime();

private:
    long startTime;

};

} // namespace blaze::lightEngine