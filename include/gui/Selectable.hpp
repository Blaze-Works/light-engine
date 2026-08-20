#pragma once

namespace blaze::lightEngine {

class Selectable {
public:
	enum class SelectionType {
		NONE,
		HOVERED,
		FOCUSED
	};

	virtual ~Selectable() = default;
	virtual SelectionType getType() const = 0;
	virtual bool isNarratable() const { return true; }
};

inline bool isFocused(Selectable::SelectionType type) {
	return type == Selectable::SelectionType::FOCUSED;
}

} // namespace blaze::lightEngine
