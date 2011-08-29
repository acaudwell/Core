#ifndef UI_SCROLL_LAYOUT_H
#define UI_SCROLL_LAYOUT_H

#include "layout.h"
#include "scroll_bar.h"

class UIScrollLayout : public UILayout {
public:
    vec2 scroll_rect;
    UIScrollBar* vertical_scrollbar;
    UIScrollBar* horizontal_scrollbar;
    
    UIScrollLayout(const vec2& scroll_rect, bool horizontal = false);
    ~UIScrollLayout();
    
    void updatePos(const vec2& pos); 
    
    void update(float dt);
    void draw();
};

#endif
