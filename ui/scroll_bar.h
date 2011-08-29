#ifndef UI_SCROLLBAR_H
#define UI_SCROLLBAR_H

#include "element.h"

class UIScrollLayout;

class UIScrollBar: public UIElement {
protected:
    std::vector<TextureResource*> bartex;
public:
    bool inverted;
    bool horizontal;
    float bar_width;
    float bar_offset;
    float bar_percent;
    vec2 bar_rect;
    
    UIScrollBar(UIScrollLayout* parent, bool horizontal = false);
    ~UIScrollBar();

    void updateRect();
    void updatePos();
    void drawContent();
};

#endif
