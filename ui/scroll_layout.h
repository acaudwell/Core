#ifndef UI_SCROLL_LAYOUT_H
#define UI_SCROLL_LAYOUT_H

#include "layout.h"
#include "scroll_bar.h"

class UIScrollLayout : public UILayout {
    vec2 scroll_rect;
    vec2 expanded_rect;
    vec2 inner_rect;
public:

    UIScrollBar* vertical_scrollbar;
    UIScrollBar* horizontal_scrollbar;

    UIScrollLayout(const vec2& scroll_rect, bool horizontal = false);
    ~UIScrollLayout();

    void mouseWheel(bool up);
    
    UIElement* elementAt(const vec2& pos);

    void updatePos(const vec2& pos);

    vec2 getRect();
    vec2 getScrollRect();
    vec2 getInnerRect();
    
    void expandRect(const vec2& expand);
    void resetRect();

    void update(float dt);
    void draw();
};

#endif
