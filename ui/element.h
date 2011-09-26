#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "../display.h"
#include "../vectors.h"
#include "../util.h"

#include <string>

#include "ui.h"

enum { UI_LABEL, UI_BUTTON, UI_IMAGE, UI_LAYOUT, UI_GROUP, UI_COLOUR, UI_SELECT, UI_SLIDER, UI_SCROLL_BAR, UI_CHECKBOX };

class UIElement {
protected:
    UI* ui;
public:
    vec2  pos;
    vec2  rect;
    bool   selected;
    bool   disabled;
    bool   hidden;
    bool   fill;

    UIElement* parent;

    // border around element between this element and other elements
    // doesnt affect the selectable area of the element
    vec4 margin;

    // padding around elements inside this element
    // affects the selectable area of the element
    vec2 padding;

    UIElement() : padding(0.0f, 0.0f), margin(0.0f, 0.0f, 0.0f, 0.0f), ui(0), disabled(false), selected(false), hidden(false), fill(false), parent(0) {};

    virtual void setUI(UI* ui) { this->ui = ui; };

    virtual ~UIElement() {};

    virtual void drawOutline();

    void drawOutline(const vec2& rect);

    void drawQuad(const vec2& rect, const vec4& texcoord);
    void drawQuad(const vec2& pos, const vec2& rect, const vec4& texcoord);

    void setPadding(const vec2& padding) { this->padding = padding; };
    void setPadding(float padding)        { this->padding = vec2(padding, padding); };

    void setMargin(const vec4& margin)  { this->margin  = margin; };
    void setMargin(const vec2& margin)  { this->margin  = vec4(margin.xy, margin.xy); };
    void setMargin(float margin)        { this->margin = vec4(margin); };

    void setPos(const vec2& pos) { this->pos = pos; };

    void hide() { hidden=true; };
    void show() { hidden=false; };
    void toggleVisibility() { hidden = !hidden; };
    
    virtual void idle() {};
    
    virtual void setSelected(bool selected) { this->selected = selected; };

    virtual bool elementsByType(std::list<UIElement*>& found, int type);

    virtual UIElement* elementAt(const vec2& pos);

    virtual vec2 getRect() { return rect; };
    
    virtual int getType() { return -1; };

    virtual void updatePos(const vec2& pos) { this->pos = pos; };
    virtual void expandRect(const vec2& expand) {};
    virtual void resetRect() {};
    
    virtual void updateContent() {};
    virtual void updateRect() {};
    
    virtual bool keyPress(SDL_KeyboardEvent *e, char c) { return false; };
    virtual bool submit() { return false; };

    virtual void update(float dt);

    virtual void mouseWheel(bool up);
   
    virtual void drag(const vec2& pos) {};
    virtual void click(const vec2& pos) { if(parent!=0) parent->click(pos); };
    virtual void doubleClick(const vec2& pos) { click(pos); };
    
    virtual void drawContent() {};

    virtual void draw();
};

#endif
