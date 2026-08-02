#pragma once

#include <vector>

namespace blaze::lightEngine {

enum VertexType {
    POS           = 0,
    POS_TEX       = 1,
    POS_TEX_COLOR = 2
};

class ShapePath {
public:
    ShapePath(VertexType t = VertexType::POS);
    ShapePath moveTo(float x, float y);
    ShapePath lineTo(float x, float y);
    ShapePath quadTo(float cx, float cy, float x, float y, int segments);
    ShapePath cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y, int segments);
    ShapePath arc(float cx, float cy, float radius, float startAngle, float endAngle, int segments);
    ShapePath color(int argb);
    ShapePath color(int r, int g, int b, int a);
    ShapePath put(float i);
    void clear();
    void endPath();
    std::vector<float> getVertices();
    const float* vertexData();
    int getVertexSize();
    int getVertexCount();
    int getBufferSize();

private:
    VertexType type;
    float currentX;
    float currentY;
    std::vector<float> vertices;
};

}
