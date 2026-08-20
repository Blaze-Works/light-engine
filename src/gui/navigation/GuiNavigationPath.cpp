#include <gui/navigation/GuiNavigationPath.hpp>
#include <gui/ParentElement.hpp>

namespace blaze::lightEngine {

std::shared_ptr<GuiNavigationPath> GuiNavigationPath::of(Element* leaf) {
	return std::make_shared<GuiNavigationLeaf>(leaf);
}

std::shared_ptr<GuiNavigationPath> GuiNavigationPath::of(ParentElement* element, std::shared_ptr<GuiNavigationPath> childPath) {
	if (!childPath) return nullptr;
	return std::make_shared<GuiNavigationIntermediaryNode>(element, childPath);
}

GuiNavigationLeaf::GuiNavigationLeaf(Element* component): component(component) {}

Element* GuiNavigationLeaf::getComponent() const {
	return component;
}

void GuiNavigationLeaf::setFocused(bool focused) {
	component->setFocused(focused);
}

GuiNavigationIntermediaryNode::GuiNavigationIntermediaryNode(ParentElement* component, std::shared_ptr<GuiNavigationPath> childPath): component(component), childPath(childPath) {}

Element* GuiNavigationIntermediaryNode::getComponent() const {
	return component;
}

std::shared_ptr<GuiNavigationPath> GuiNavigationIntermediaryNode::getChildPath() const {
	return childPath;
}

void GuiNavigationIntermediaryNode::setFocused(bool focused) {
	if (!focused) component->setFocused(nullptr);
	else component->setFocused(childPath->getComponent());
	childPath->setFocused(focused);
}

} // namespace blaze::lightEngine
