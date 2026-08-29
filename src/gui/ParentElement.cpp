#include <gui/navigation/GuiNavigation.hpp>
#include <gui/navigation/GuiNavigationPath.hpp>
#include <gui/ParentElement.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <memory>

namespace blaze::lightEngine {

bool ParentElement::mouseClicked(double mouseX, double mouseY, int button) {
	for (Element* element : this->getChildren()) {
		if (element->mouseClicked(mouseX, mouseY, button)) {
			this->setFocused(element);
			if (button == 0) this->setDragging(true);
			return true;
		}
	}
	return false;
}

bool ParentElement::mouseReleased(double mouseX, double mouseY, int button) {
	this->setDragging(false);
	auto hovered = this->hoveredElement(mouseX, mouseY);
	if (hovered.has_value()) {
		Element* element = hovered.value();
		return element->mouseReleased(mouseX, mouseY, button);
	}
	return false;
}

bool ParentElement::mouseDragged(double mouseX, double mouseY, int button, double deltaX, double deltaY) {
	Element* focused = this->getFocused();
	return focused != nullptr && this->isDragging() && button == 0 && focused->mouseDragged(mouseX, mouseY, button, deltaX, deltaY);
}

bool ParentElement::mouseScrolled(double mouseX, double mouseY, double horizontalAmount, double verticalAmount) {
	auto hovered = this->hoveredElement(mouseX, mouseY);
	if (hovered.has_value()) {
		return hovered.value()->mouseScrolled(mouseX, mouseY, horizontalAmount, verticalAmount);
	}
	return false;
}

bool ParentElement::keyPressed(int keyCode, int scanCode, int modifiers) {
	Element* focused = this->getFocused();
	return focused != nullptr && focused->keyPressed(keyCode, scanCode, modifiers);
}

bool ParentElement::keyReleased(int keyCode, int scanCode, int modifiers) {
	Element* focused = this->getFocused();
	return focused != nullptr && focused->keyReleased(keyCode, scanCode, modifiers);
}

bool ParentElement::charTyped(char chr, int modifiers) {
	Element* focused = this->getFocused();
	return focused != nullptr && focused->charTyped(chr, modifiers);
}

bool ParentElement::isFocused() const {
	return this->getFocused() != nullptr;
}

std::shared_ptr<GuiNavigationPath> ParentElement::getFocusedPath() const {
	Element* f = this->getFocused();
	if (f != nullptr) return f->getFocusedPath();
	return nullptr;
}

std::shared_ptr<GuiNavigationPath> ParentElement::getNavigationPath(const GuiNavigation& navigation) {
	Element* f = this->getFocused();
	if (f != nullptr) {
		std::shared_ptr<GuiNavigationPath> path = f->getNavigationPath(navigation);
		if (path != nullptr) return GuiNavigationPath::of(this, path);
	}

	const GuiNavigation* navigation_ptr = &navigation;

	if (auto tab = dynamic_cast<const GuiNavigationTab*>(navigation_ptr)) {
		return this->computeNavigationPath(*tab);
	} else if (auto arrow = dynamic_cast<const GuiNavigationArrow*>(navigation_ptr)) {
		return this->computeNavigationPath(*arrow);
	}

	return nullptr;
}

std::shared_ptr<GuiNavigationPath> ParentElement::computeNavigationPath(const GuiNavigationTab& navigation) {
	bool bl = navigation.getForward();
	Element* element = this->getFocused();
	std::vector<Element*> vec(this->getChildren());
	std::sort(vec.begin(), vec.end(), [](Element* a, Element* b) {
		return a->getNavigationOrder() < b->getNavigationOrder();
	});
	
	int i = std::find(vec.begin(), vec.end(), element) - vec.begin(), j = 0;

	if (element != nullptr && i >= 0) j = i + bl;
	else if (bl) j = 0;
	else j = vec.size();

	auto it = vec.begin();
	std::advance(it, j);

	std::function<bool()> tempBoolSupplier;

	if (bl) tempBoolSupplier = [&it, &vec]() {
		return it != vec.end();
	};
	else tempBoolSupplier = [&it, &vec]() {
		return it != vec.begin();
	};

	std::function<bool()> boolSupplier = tempBoolSupplier;
	std::function<Element*()> tempSupplier;

	if (bl) tempSupplier = [&it]() -> Element* {
		return *it++;
	};
	else tempSupplier = [&it]() -> Element* {
		return *(--it);
	};

	std::function<Element*()> supplier = tempSupplier;
	std::shared_ptr<GuiNavigationPath> guiNavigationPath;
	do {
		if (!boolSupplier()) {
			return nullptr;
		}

		Element* element2 = supplier();
		guiNavigationPath = element2->getNavigationPath(navigation);
	} while (guiNavigationPath == nullptr);

	return GuiNavigationPath::of(this, guiNavigationPath);
}

std::shared_ptr<GuiNavigationPath> ParentElement::computeNavigationPath(const GuiNavigationArrow& navigation) {
	Element* element = this->getFocused();
	if (element == nullptr) {
		NavigationDirection navigationDirection = navigation.getDirection();
		ScreenRect screenRect = this->getNavigationFocus().getBorder(getOpposite(navigationDirection));
		return GuiNavigationPath::of(this, this->computeChildPath(screenRect, navigationDirection, nullptr, navigation));
	} else {
		ScreenRect screenRect2 = element->getNavigationFocus();
		return GuiNavigationPath::of(this, this->computeChildPath(screenRect2, navigation.getDirection(), element, navigation));
	}
}

std::shared_ptr<GuiNavigationPath> ParentElement::computeChildPath(const ScreenRect& focus, const NavigationDirection& direction, Element* focused, const GuiNavigation& navigation) {
	NavigationAxis navigationAxis = getAxis(direction);
	NavigationAxis navigationAxis2 = getOther(navigationAxis);
	NavigationDirection navigationDirection = getPositiveDirection(navigationAxis2);
	int i = focus.getBoundingCoordinate(getOpposite(direction));
	std::vector<Element*> vec;

	for (auto element : this->getChildren()) {
		if (element != focused) {
			ScreenRect screenRect = element->getNavigationFocus();
			if (screenRect.overlaps(focus, navigationAxis2)) {
				int j = screenRect.getBoundingCoordinate(getOpposite(direction));
				if (isAfter(direction, j, i)) {
					vec.push_back(element);
				} else if (j == i && isAfter(direction, screenRect.getBoundingCoordinate(direction), focus.getBoundingCoordinate(direction))) {
					vec.push_back(element);
				}
			}
		}
	}

	std::sort(vec.begin(), vec.end(), [&direction](Element* a, Element* b) {
		std::function<int(int,int)> x = getComparator(direction);
		int coordA = a->getNavigationFocus().getBoundingCoordinate(getOpposite(direction));
		int coordB = b->getNavigationFocus().getBoundingCoordinate(getOpposite(direction));
		return x(coordA, coordB);
	});

	std::sort(vec.begin(), vec.end(), [&navigationDirection](Element* a, Element* b) {
		std::function<int(int,int)> x = getComparator(navigationDirection);
		int coordA = a->getNavigationFocus().getBoundingCoordinate(getOpposite(navigationDirection));
		int coordB = b->getNavigationFocus().getBoundingCoordinate(getOpposite(navigationDirection));
		return x(coordA, coordB);
	});

	auto it = vec.begin();

	std::shared_ptr<GuiNavigationPath> guiNavigationPath;
	do {
		if (it == vec.end()) {
			return this->computeInitialChildPath(focus, direction, focused, navigation);
		}

		Element* element2 = *it;
		guiNavigationPath = element2->getNavigationPath(navigation);
	} while(guiNavigationPath == nullptr);

	return guiNavigationPath;
}

std::shared_ptr<GuiNavigationPath> ParentElement::computeInitialChildPath(const ScreenRect& focus, const NavigationDirection& direction, Element* focused, const GuiNavigation& navigation) {
	NavigationAxis navigationAxis = getAxis(direction);
	NavigationAxis navigationAxis2 = getOther(navigationAxis);
	std::vector<std::pair<Element*, long>> vec;
	ScreenPos screenPos = ScreenPos::of(navigationAxis, focus.getBoundingCoordinate(direction), focus.getCenter(navigationAxis2));
	std::vector<Element*> children = this->getChildren();
	auto it = children.begin();

	while(it != this->getChildren().end()) {
		Element* element = *it;
		if (element != focused) {
			ScreenRect screenRect = element->getNavigationFocus();
			ScreenPos screenPos2 = ScreenPos::of(navigationAxis, screenRect.getBoundingCoordinate(getOpposite(direction)), screenRect.getCenter(navigationAxis2));
			if (isAfter(direction, screenPos2.getComponent(navigationAxis), screenPos.getComponent(navigationAxis))) {
				long l = glm::distance2(glm::vec2(screenPos.getX(), screenPos.getY()), glm::vec2(screenPos2.getX(), screenPos2.getY()));
				vec.push_back(std::pair<Element*, long>(element, l));
			}
		}
	}

	std::sort(vec.begin(), vec.end(), [](std::pair<Element*, long> a, std::pair<Element*, long> b) {
		return a.second > b.second;
	});

	auto it2 = vec.begin();

	std::shared_ptr<GuiNavigationPath> guiNavigationPath;
	do {
		if (it2 == vec.end()) {
			return nullptr;
		}

		std::pair<Element*, long> pair = *it2;
		guiNavigationPath = (pair.first)->getNavigationPath(navigation);
	} while(guiNavigationPath == nullptr);

	return guiNavigationPath;
}

} // namespace blaze::lightEngine
