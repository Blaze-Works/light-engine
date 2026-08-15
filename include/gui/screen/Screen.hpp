#pragma once

#include <gui/AbstractParentElement.hpp>
#include <gui/Drawable.hpp>
#include <gui/ScreenRect.hpp>
#include <gui/Selectable.hpp>
#include <gui/navigation/GuiNavigationPath.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace blaze::lightEngine {

class LightEngine;
class DrawContext;

class Screen : public AbstractParentElement, public Drawable {
public:
    explicit Screen(std::string title);
    virtual ~Screen() = default;

    const std::string& getTitle() const;
    void render(DrawContext& context, int mouseX, int mouseY, float delta) override;
    bool keyPressed(int keyCode, int scanCode, int modifiers) override;
    void init(LightEngine& engine, int width, int height);
    std::vector<Element*> getChildren() override;
    void removed();
    void onDisplayed();
    bool shouldPause() const;
    void resize(int width, int height);
    bool isMouseOver(double mouseX, double mouseY) override;
    ScreenRect getNavigationFocus() const override;
    void blur();

protected:
    virtual void init();
    virtual void refreshWidgetPositions();
    virtual void clearAndInit();
    virtual void setInitialFocus();
    void setInitialFocus(Element* element);
    void switchFocus(const std::shared_ptr<GuiNavigationPath>& path);
    bool shouldCloseOnEsc() const;
    virtual void close();
    virtual void insertText(const std::string& text, bool overrideText);

    template<typename T>
    T* addDrawableChild(std::unique_ptr<T> element) {
        static_assert(std::is_base_of_v<Element, T>, "T must derive from Element");
        static_assert(std::is_base_of_v<Drawable, T>, "T must derive from Drawable");
        static_assert(std::is_base_of_v<Selectable, T>, "T must derive from Selectable");

        T* raw = element.get();
        this->owned.push_back(std::move(element));
        this->drawables.push_back(raw);
        this->selectables.push_back(raw);
        this->children.push_back(raw);
        return raw;
    }

    template<typename T, typename... Args>
    T* addDrawableChild(Args&&... args) {
        return this->addDrawableChild(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template<typename T>
    T* addDrawable(std::unique_ptr<T*> drawable) {
        static_assert(std::is_base_of_v<Drawable, T>, "T must derive from Drawable");

        T* raw = drawable.get();
        this->owned.push_back(drawable);
        this->drawables.push_back(raw);
        return raw;
    }

    template<typename T, typename... Args>
    T* addDrawable(Args&&... args) {
        return this->addDrawable(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template<typename T>
    T* addSelectableChild(std::unique_ptr<T*> child) {
        static_assert(std::is_base_of_v<Element, T>, "T must derive from Element");
        static_assert(std::is_base_of_v<Selectable, T>, "T must derive from Selectable");

        T* raw = child.get();
        this->children.push_back(raw);
        this->selectables.push_back(raw);
        return raw;
    }

    template<typename T, typename... Args>
    T* addSelectableChild(std::unique_ptr<T*>, Args&&... args) {
        return this->addSelectableChild(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void remove(Element* child);
    void clearChildren();
    LightEngine* engine = nullptr;

private:
    std::shared_ptr<GuiNavigation> getGuiNavigation(int keyCode);
    std::shared_ptr<GuiNavigationTab> getTabNavigation();
    std::shared_ptr<GuiNavigationArrow> getArrowNavigation(const NavigationDirection& direction);
    int getNavigationOrder() const override;

    std::string title;
    bool screenInitialized = false;
    int width = 0;
    int height = 0;

    std::vector<std::unique_ptr<Element>> owned;
    std::vector<Drawable*> drawables;
    std::vector<Element*> children;
    std::vector<Selectable*> selectables;
};

} // namespace blaze::lightEngine
