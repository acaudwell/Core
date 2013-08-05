#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "../display.h"
#include "../vectors.h"

#include "ui.h"

#include <string>

enum { UI_INVALID, UI_ELEMENT, UI_LABEL, UI_BUTTON, UI_IMAGE, UI_LAYOUT, UI_GROUP, UI_COLOUR, UI_SELECT, UI_SLIDER, UI_SCROLL_BAR, UI_CHECKBOX };

class UIElement {
protected:
    UI* ui;
    bool   fill_horizontal;
    bool   fill_vertical;
public:
    vec2  pos;
    vec2  rect;
    bool  editable;
    bool  selected;
    bool  disabled;
    bool  hidden;
    bool  scrollable;
    bool  selectable;

    int zindex;

    UIElement* parent;

    // border around element between this element and other elements
    // doesnt affect the selectable area of the element
    vec4 margin;

    // padding around elements inside this element
    // affects the selectable area of the element
    vec2 padding;

    UIElement() : padding(0.0f, 0.0f), margin(0.0f, 0.0f, 0.0f, 0.0f), ui(0), disabled(false), selected(false), editable(false), hidden(false), fill_horizontal(false), fill_vertical(false), scrollable(false), selectable(true), zindex(0), parent(0) {};

    virtual void setUI(UI* ui) { this->ui = ui; };
    virtual ~UIElement();

    virtual void drawOutline();

    void drawOutline(const vec2& rect);

    static void getModifiers(bool& left_ctrl, bool& left_shift);
    static double granulaity(double initial, double scale);

    void drawQuad(const vec2& rect, const vec4& texcoord);
    void drawQuad(const vec2& pos, const vec2& rect, const vec4& texcoord);

    void setPadding(const vec2& padding) { this->padding = padding; };
    void setPadding(float padding)        { this->padding = vec2(padding, padding); };

    void setMargin(const vec4& margin)  { this->margin = margin; };
    void setMargin(const vec2& margin)  { this->margin = vec4(margin.x, margin.y, margin.x, margin.y); };
    void setMargin(float margin)        { this->margin = vec4(margin); };

    virtual vec4 getMargin() const { return margin; };

    void setFillHorizontal(bool fill) { this->fill_horizontal = fill; };
    void setFillVertical  (bool fill) { this->fill_vertical  = fill; };
    void setFill(bool fill)           { this->fill_horizontal = fill; this->fill_vertical = fill; };
   
    bool fillVertical()   const { return fill_vertical; }
    bool fillHorizontal() const { return fill_horizontal; }

    virtual bool isVisible() const { return !hidden; }

    void setPos(const vec2& pos) { this->pos = pos; };

    void hide() { hidden=true; };
    void show() { hidden=false; };

    virtual void idle() {};

    virtual void setText(const std::string& text) {};

    virtual void setSelected(bool selected) { this->selected = selected; };

    virtual bool elementsByType(std::list<UIElement*>& found, int type);

    virtual void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    virtual vec2 getRect() { return rect; };

    virtual int getType() const { return UI_ELEMENT; };

    const std::string& getElementName() const;
    static const std::string& getElementName(int type);

    virtual void updatePos(const vec2& pos) { this->pos = pos; };
    virtual void expandRect(const vec2& expand) {};
    virtual void resetRect() {};

    virtual void updateContent() {};
    virtual void updateRect() {};
    virtual void updateZIndex();

    virtual bool keyPress(SDL_KeyboardEvent *e) { return false; };
    virtual bool submit() { return false; };

    virtual void update(float dt);

    virtual void scroll(bool up);

    virtual void drag(const vec2& pos) {};
    virtual void click(const vec2& pos) { if(parent!=0) parent->click(pos); };
    virtual void doubleClick(const vec2& pos) { click(pos); };

    virtual bool isEditable()   { return editable; };
    virtual bool isScrollable() { return scrollable; };
    virtual bool isSelectable() { return selectable; };

    virtual void drawContent() {};

    virtual void draw();

    static bool reverse_zindex_sort (UIElement* e1, UIElement* e2) {
      if(e1->zindex != e2->zindex) return e1->zindex < e2->zindex;
      return e1 < e2;
    }

    static bool zindex_sort (UIElement* e1, UIElement* e2) {
      if(e2->zindex != e1->zindex) return e2->zindex < e1->zindex;
      return e1 < e2;
    }
};

#endif
