#pragma once

namespace blaze::lightEngine {

class ClickableMouseListener {
public:
    virtual ~ClickableMouseListener() = default;
    virtual void onMouseDown(int button, double x, double y) = 0;
    virtual void onMouseUp(int button, double x, double y) = 0;
};

} // namespace blaze::lightEngine
