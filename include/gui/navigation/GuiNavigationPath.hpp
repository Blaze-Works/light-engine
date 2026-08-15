#pragma once

#include <gui/Element.hpp>
#include <memory>

namespace blaze::lightEngine {

class ParentElement;

class GuiNavigationPath {
public:
    virtual ~GuiNavigationPath() = default;
    virtual Element* getComponent() const = 0;
    virtual void setFocused(bool focused) = 0;

    static std::shared_ptr<GuiNavigationPath> of(Element* leaf);
    static std::shared_ptr<GuiNavigationPath> of(ParentElement* element, std::shared_ptr<GuiNavigationPath> childPath);
};

class GuiNavigationLeaf : public GuiNavigationPath {
public:
    explicit GuiNavigationLeaf(Element* component);
    Element* getComponent() const override;
    void setFocused(bool focused) override;

private:
    Element* component;
};

class GuiNavigationIntermediaryNode : public GuiNavigationPath {
public:
    GuiNavigationIntermediaryNode(ParentElement* component, std::shared_ptr<GuiNavigationPath> childPath);
    Element* getComponent() const override;
    void setFocused(bool focused) override;
    std::shared_ptr<GuiNavigationPath> getChildPath() const;

private:
    ParentElement* component;
    std::shared_ptr<GuiNavigationPath> childPath;
};

} // namespace blaze::lightEngine
