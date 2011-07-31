#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "../display.h"
#include "../vectors.h"
#include "../util.h"

#include <string>

#include "ui.h"

enum { UI_LABEL, UI_IMAGE, UI_LAYOUT, UI_GROUP, UI_COLOUR, UI_SLIDER, UI_CHECKBOX };

class UIElement {
protected:
    UI* ui;
public:
    vec2f  pos;
    vec2f  rect;
    vec4f  background;
    bool   selected;
    bool   disabled;
    UIElement* parent;

    // border around element between this element and other elements
    // doesnt affect the selectable area of the element
    vec2f margin;

    // padding around elements inside this element
    // affects the selectable area of the element
    vec2f padding;

    UIElement() : padding(0.0f, 0.0f), margin(0.0f, 0.0f), ui(0), disabled(false), selected(false), parent(0) {};

    virtual void setUI(UI* ui) { this->ui = ui; };

    virtual ~UIElement() {};

    virtual void drawOutline();

    void drawOutline(const vec2f& rect);

    void drawQuad(const vec2f& rect, const vec4f& texcoord);
    void drawQuad(const vec2f& pos, const vec2f& rect, const vec4f& texcoord);

    void setPadding(const vec2f& padding) { this->padding = padding; };
    void setPadding(float padding)        { this->padding = vec2f(padding, padding); };

    void setMargin(const vec2f& margin)  { this->margin  = margin; };
    void setMargin(float margin)         { this->margin = vec2f(margin, margin); };

    void setPos(const vec2f& pos) { this->pos = pos; };

    virtual void setSelected(bool selected) { this->selected = selected; };

    virtual bool elementsByType(std::list<UIElement*>& found, int type);

    virtual UIElement* elementAt(const vec2f& pos);

    virtual int getType() { return -1; };

    virtual void updatePos(const vec2f& pos) { this->pos = pos; };
    virtual void updateRect() {};

    virtual void update(float dt);

    virtual void mouseWheel(bool up) {};

    virtual void drawContent() {};

    virtual void draw();
};

#endif
