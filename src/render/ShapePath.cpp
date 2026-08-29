#include <color/Argb.hpp>
#include <render/ShapePath.hpp>

#include <cmath>

namespace blaze::lightEngine {

ShapePath::ShapePath(VertexType t): type(t), currentX(0), currentY(0) {}

ShapePath ShapePath::moveTo(float x, float y) {
	this->vertices.push_back(x);
	this->vertices.push_back(y);
	this->currentX = x;
	this->currentY = y;
	return *this;
}

ShapePath ShapePath::lineTo(float x, float y) {
	this->vertices.push_back(x);
	this->vertices.push_back(y);
	this->currentX = x;
	this->currentY = y;
	return *this;
}

ShapePath ShapePath::quadTo(float cx, float cy, float x, float y, int segments) {
	for (int i = 1; i <= segments; i++) {
		float t = i / (float) segments;
		float xt = (1 - t) * (1 - t) * currentX + 2 * (1 - t) * t * cx + t * t * x;
		float yt = (1 - t) * (1 - t) * currentY + 2 * (1 - t) * t * cy + t * t * y;
		this->put(xt).put(yt);
	}
	this->currentX = x;
	this->currentY = y;

	return *this;
}

ShapePath ShapePath::cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y, int segments) {
	for (int i = 1; i <= segments; i++) {
		float t = i / (float) segments;
		float xt = (float)(std::pow(1 - t, 3) * currentX + 3 * std::pow(1 - t, 2) * t * cx1 + 3 * (1 - t) * t * t * cx2 + t * t * t * x);
		float yt = (float)(std::pow(1 - t, 3) * currentY + 3 * std::pow(1 - t, 2) * t * cy1 + 3 * (1 - t) * t * t * cy2 + t * t * t * y);
		this->put(xt).put(yt);
	}
	this->currentX = x;
	this->currentY = y;

	return *this;
}

ShapePath ShapePath::arc(float cx, float cy, float radius, float startAngle, float endAngle, int segments) {
	for (int i = 0; i <= segments; i++) {
		float theta = startAngle + i * (endAngle - startAngle) / segments;
		float x = cx + (float)std::cos(theta) * radius;
		float y = cy + (float)std::sin(theta) * radius;
		this->put(x).put(y);
	}
	this->currentX = this->vertices[this->getVertexCount() - this->getVertexSize() - 1];
	this->currentY = this->vertices[this->getVertexCount() - this->getVertexSize()];

	return *this;
}

ShapePath ShapePath::color(int argb) {
	return this->color(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

ShapePath ShapePath::color(int r, int g, int b, int a) {
	this->put((float) r / 255).put((float) g / 255).put((float) b / 255).put((float) a / 255);
	return *this;
}

ShapePath ShapePath::put(float i) {
	this->vertices.push_back(i);
	return *this;
}

void ShapePath::clear() {
	this->vertices.clear();
}

void ShapePath::endPath() {
	this->vertices.clear();
}

int ShapePath::getVertexCount() {
	return static_cast<int>(this->vertices.size() / this->getVertexSize());
}

int ShapePath::getVertexSize() {
	switch (this->type) {
		case(VertexType::POS_TEX_COLOR): return 8;
		case(VertexType::POS_TEX):       return 4;
		case(VertexType::POS):           return 2;
		default:                         return 0;
	}
}

int ShapePath::getBufferSize() {
	return this->vertices.size() * sizeof(float);
}

std::vector<float> ShapePath::getVertices() {
	return this->vertices;
}

const float* ShapePath::vertexData() {
	return this->vertices.data();
}

} // namespace blaze::lightEngine
