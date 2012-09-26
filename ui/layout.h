#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "element.h"

enum {
    UI_LAYOUT_ALIGN_NONE,
    UI_LAYOUT_ALIGN_TOP_LEFT,
    UI_LAYOUT_ALIGN_BOTTOM_LEFT,
    UI_LAYOUT_ALIGN_TOP_RIGHT,
    UI_LAYOUT_ALIGN_BOTTOM_RIGHT
};

class UILabel;

class UILayout : public UIElement {
protected:
    int alignment;
    bool horizontal;
    std::vector<UIElement*> elements;

    vec2 min_rect;
    vec2 expanded_rect;
    bool centre;
    bool drawbg;

    virtual void drawBackground();
public:
    vec4 bgcolour;

    UILayout(bool horizontal = false);
    ~UILayout();

    void clear();

    int getType() const { return UI_LAYOUT; };

    vec2 getInnerRect();
    vec2 getRect();

    void expandRect(const vec2& expand);
    void resetRect();

    void setUI(UI* ui);

    void addElement(UIElement* e);

    void setDrawBackground(bool drawbg);

    void setHorizontal(bool horizontal);
    void setAlignment(int alignment) { this->alignment = alignment; };

    void setMinRect(const vec2& min_rect);

    int getElementCount() { return elements.size(); };

    UIElement* getElement(int index) { return elements[index]; };

    bool elementsByType(std::list<UIElement*>& found, int type);

    void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    virtual void resize(const vec2& pos);

    void updatePos(const vec2& pos);

    void update(float dt);

    void draw();
    void drawOutline();
};


class UILabelledElement : public UILayout {
protected:
    UILayout* layout;
    UILabel*  label;
public:
    UILabelledElement(const std::string text, UIElement* e = 0, float width = -1.0f);

    UILabel*  getLabel();
    UILayout* getLayout();
};


class UIResizeButton : public UIElement {
protected:
    TextureResource* resizetex;
public:
    UIResizeButton();
    ~UIResizeButton();

    void drag(const vec2& pos);
    void drawContent();
};

class UIResizableLayout : public UILayout {
    UIResizeButton* resize_button;
public:
    UIResizableLayout(bool horizontal = false);
    ~UIResizableLayout();

    void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    void setUI(UI* ui);
    void updatePos(const vec2& pos);
    void update(float dt);
    void draw();
};

#endif
