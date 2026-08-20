#include <gui/navigation/GuiNavigation.hpp>
#include <gui/navigation/NavigationDirection.hpp>
#include <gui/screen/Screen.hpp>
#include <LightEngine.hpp>

#include <algorithm>
#include <memory>

namespace blaze::lightEngine {

Screen::Screen(std::string title) : title(std::move(title)) {}

const std::string& Screen::getTitle() const {
	return this->title;
}

void Screen::render(DrawContext& context, int mouseX, int mouseY, float delta) {
	for (Drawable* drawable : this->drawables) {
		drawable->render(context, mouseX, mouseY, delta);
	}
}

void Screen::init(LightEngine& engine, int width, int height) {
	this->engine = &engine;
	this->width = width;
	this->height = height;

	if (!this->screenInitialized) {
		this->init();
		this->setInitialFocus();
	} else {
		this->refreshWidgetPositions();
	}

	this->screenInitialized = true;
}

std::vector<Element*> Screen::getChildren() {
	return this->children;
}

void Screen::removed() {}

void Screen::onDisplayed() {}

bool Screen::shouldPause() const {
	return true;
}

void Screen::resize(int width, int height) {
	this->width = width;
	this->height = height;
	this->refreshWidgetPositions();
}

bool Screen::isMouseOver(double mouseX, double mouseY) {
	return this->getNavigationFocus().contains(mouseX, mouseY);
}

ScreenRect Screen::getNavigationFocus() const {
	return ScreenRect(0, 0, this->width, this->height);
}

void Screen::init() {}

void Screen::update(float /*delta*/) {}

void Screen::refreshWidgetPositions() {
	this->clearAndInit();
}

void Screen::clearAndInit() {
	this->blur();
	this->setFocused(nullptr);
	this->clearChildren();
	this->init();
	this->setInitialFocus();
}

void Screen::setInitialFocus() {
	if (isKeyboard(this->engine->getNavigationType())) {
		GuiNavigationTab tab(true);
		std::shared_ptr<GuiNavigationPath> guiNavigationPath = this->getNavigationPath(tab);
		if (guiNavigationPath != nullptr) {
			this->switchFocus(guiNavigationPath);
		}
	}
}

void Screen::setInitialFocus(Element* element) {
	if (!element) return;
	std::shared_ptr<GuiNavigationPath> path = GuiNavigationPath::of(this, element->getNavigationPath(GuiNavigationDown()));
	if (path) {
		this->switchFocus(path);
	}
}

bool Screen::keyPressed(int keyCode, int scanCode, int modifiers) {
	if (keyCode == 256 && this->shouldCloseOnEsc()) {
		this->close();
		return true;
	}

	if (ParentElement::keyPressed(keyCode, scanCode, modifiers)) {
		return true;
	}

	std::shared_ptr<GuiNavigation> navigation = this->getGuiNavigation(keyCode);

	if (navigation != nullptr) {
		std::shared_ptr<GuiNavigationPath> path = this->getNavigationPath(*navigation);
		if (!path && dynamic_cast<GuiNavigationTab*>(navigation.get()) != nullptr) {
			this->blur();
			path = this->getNavigationPath(*navigation);
		}

		if (path) this->switchFocus(path);
	}

	return false;
}

std::shared_ptr<GuiNavigation> Screen::getGuiNavigation(int keyCode) {
	std::shared_ptr<GuiNavigation> navigation;

	switch (keyCode) {
		case 258:
			navigation = this->getTabNavigation();
			break;
		default:
			navigation = nullptr;
			break;
		case 262:
			navigation = this->getArrowNavigation(NavigationDirection::RIGHT);
			break;
		case 263:
			navigation = this->getArrowNavigation(NavigationDirection::LEFT);
			break;
		case 264:
			navigation = this->getArrowNavigation(NavigationDirection::DOWN);
			break;
		case 265:
			navigation = this->getArrowNavigation(NavigationDirection::UP);
	}

	return navigation;
}

std::shared_ptr<GuiNavigationTab> Screen::getTabNavigation() {
	return std::make_shared<GuiNavigationTab>(true);
}

std::shared_ptr<GuiNavigationArrow> Screen::getArrowNavigation(const NavigationDirection& direction) {
	return std::make_shared<GuiNavigationArrow>(direction);
}

int Screen::getNavigationOrder() const {
	return 0;
}

void Screen::blur() {
	std::shared_ptr<GuiNavigationPath> path = this->getFocusedPath();
	if (path) {
		path->setFocused(false);
	}
}

void Screen::switchFocus(const std::shared_ptr<GuiNavigationPath>& path) {
	this->blur();
	if (path) {
		path->setFocused(true);
	}
}

bool Screen::shouldCloseOnEsc() const {
	return true;
}

void Screen::close() {
	this->engine->popScreen();
}

void Screen::insertText(const std::string& /*text*/, bool /*overrideText*/) {}

void Screen::remove(Element* child) {
	if (!child) return;

	auto drawable = dynamic_cast<Drawable*>(child);
	auto selectable = dynamic_cast<Selectable*>(child);

	if (drawable) this->drawables.erase(std::remove(this->drawables.begin(), this->drawables.end(), drawable), this->drawables.end());
	if (selectable) this->selectables.erase(std::remove(this->selectables.begin(), this->selectables.end(), selectable), this->selectables.end());
	this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());

	this->owned.erase(std::remove_if(this->owned.begin(), this->owned.end(), [child](const std::unique_ptr<Element>& ptr) { return ptr.get() == child; }), this->owned.end());
}

void Screen::clearChildren() {
	this->drawables.clear();
	this->children.clear();
	this->selectables.clear();
	this->owned.clear();
}

} // namespace blaze::lightEngine
