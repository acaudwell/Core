#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "element.h"
#include "label.h"

enum {
    UI_LAYOUT_ALIGN_NONE,
    UI_LAYOUT_ALIGN_TOP_LEFT,
    UI_LAYOUT_ALIGN_BOTTOM_LEFT,
    UI_LAYOUT_ALIGN_TOP_RIGHT,
    UI_LAYOUT_ALIGN_BOTTOM_RIGHT
};

class UILayout : public UIElement {
protected:
    int alignment;
    bool horizontal;
    std::vector<UIElement*> elements;
public:
    vec4 background;

    UILayout(bool horizontal = false);
    ~UILayout();

    void clear();
    
    int getType() { return UI_LAYOUT; };

    void setUI(UI* ui);

    void addElement(UIElement* e);

    void setHorizontal(bool horizontal);
    void setAlignment(int alignment) { this->alignment = alignment; };

    int getElementCount() { return elements.size(); };

    UIElement* getElement(int index) { return elements[index]; };

    bool elementsByType(std::list<UIElement*>& found, int type);

    UIElement* elementAt(const vec2& pos);
    
    void updatePos(const vec2& pos);
    
    void update(float dt);

    void draw();
    void drawOutline();
};


class UILabelledElement : public UILayout {
public:
    UILabelledElement(const std::string text, UIElement* e, float width = 120.0f);
};

#endif
