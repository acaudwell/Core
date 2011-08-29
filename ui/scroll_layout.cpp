#include "scroll_layout.h"

UIScrollLayout::UIScrollLayout(const vec2& scroll_rect, bool horizontal) : scroll_rect(scroll_rect), UILayout(horizontal) {

    vertical_scrollbar   = new UIScrollBar(this, false);
    horizontal_scrollbar = new UIScrollBar(this, true);
}

UIScrollLayout::~UIScrollLayout() {
    delete vertical_scrollbar;
    delete horizontal_scrollbar;
}

void UIScrollLayout::update(float dt) {

    UILayout::update(dt);
    
    rect = scroll_rect;//glm::min( scroll_rect, rect );

    vertical_scrollbar->update(dt);
    horizontal_scrollbar->update(dt);
}

void UIScrollLayout::updatePos(const vec2& pos) {

    UILayout::updatePos(pos);
    
    vertical_scrollbar->updatePos();
    horizontal_scrollbar->updatePos();
}

void UIScrollLayout::draw() {

    glEnable(GL_SCISSOR_TEST);
    
    glScissor(pos.x, display.height-(pos.y+rect.y), rect.x, rect.y);    

    UILayout::draw();
    
    glDisable(GL_SCISSOR_TEST);
    
    vertical_scrollbar->bar_percent = 0.5f;
    horizontal_scrollbar->bar_percent = 0.5f;
    
    vertical_scrollbar->draw();
    horizontal_scrollbar->draw();    
}