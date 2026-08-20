#pragma once

#include <cstdint>

namespace blaze::lightEngine {

class Layers {
public:
	Layers() : mask(1) {}

	void set(int channel) { mask = 1u << channel; }
	void enable(int channel) { mask |= 1u << channel; }
	void enableAll() { mask = 0xffffffffu; }
	void toggle(int channel) { mask ^= 1u << channel; }
	void disable(int channel) { mask &= ~(1u << channel); }
	void disableAll() { mask = 0; }

	bool test(const Layers& other) const { return (mask & other.mask) != 0; }

	std::uint32_t mask;
};

} // namespace blaze::lightEngine
