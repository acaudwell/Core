#ifndef UI_SCROLL_LAYOUT_H
#define UI_SCROLL_LAYOUT_H

#include "layout.h"
#include "scroll_bar.h"

class UIScrollLayout : public UILayout {
protected:
    vec2 scroll_rect;
    vec2 inner_rect;

    void drawBackground();
public:

    UIScrollBar* vertical_scrollbar;
    UIScrollBar* horizontal_scrollbar;

    UIScrollLayout(const vec2& scroll_rect, bool horizontal = false);
    ~UIScrollLayout();

    void scroll(bool up);

    bool isScrollable();

    void updateZIndex();

    void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    void updatePos(const vec2& pos);

    vec2 getRect();
    vec2 getScrollRect();
    vec2 getInnerRect();
    
    vec4 getMargin() const;
    
    void setUI(UI* ui);

    void update(float dt);
    void draw();
};

#endif
