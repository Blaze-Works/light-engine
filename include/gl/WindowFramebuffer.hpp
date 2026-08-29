#pragma once

#include <gl/Framebuffer.hpp>

#include <array>
#include <string>
#include <vector>

namespace blaze::lightEngine {

enum Attachment: uint8_t {
	NONE        = 0,
	COLOR       = 1,
	DEPTH       = 2,
	COLOR_DEPTH = 3
};

inline Attachment operator|(Attachment lhs, Attachment rhs) {
	return static_cast<Attachment>(
		static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
	);
}

struct AttachmentHelper {
	static constexpr std::array<Attachment, 4> VALUES = {
		Attachment::NONE,
		Attachment::COLOR,
		Attachment::DEPTH,
		Attachment::COLOR_DEPTH
	};

	static Attachment with(Attachment current, Attachment other) {
		size_t index = static_cast<size_t>(current) | static_cast<size_t>(other);
		return VALUES.at(index);
	}

	static std::string name(Attachment attachment) {
		switch (attachment) {
			case Attachment::NONE:        return "NONE";
			case Attachment::COLOR:       return "COLOR";
			case Attachment::DEPTH:       return "DEPTH";
			case Attachment::COLOR_DEPTH: return "COLOR_DEPTH";
		}
		return "UNKNOWN";
	}
};

struct Size {
	int width;
	int height;

	Size(int width, int height);
	static std::vector<Size> findCompatible(int width, int height);
	bool equals(Size other);
	std::string toString();
};

class WindowFramebuffer: public Framebuffer {
public:
	WindowFramebuffer(int width, int height);

	static const int DEFAULT_WIDTH = 854;
	static const int DEFAULT_HEIGHT = 480;

private:
	void init(int width, int height);
	Size* findSuitableSize(int width, int height);
	bool supportsColor(Size* size);
	bool supportsDepth(Size* size);

	GLuint colorAttachment;
	GLuint depthAttachment;
	int viewportWidth;
	int viewportHeight;
	int textureWidth;
	int textureHeight;
};

} // namespace blaze::lightEngine
