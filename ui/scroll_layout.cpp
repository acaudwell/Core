#include "scroll_layout.h"

UIScrollLayout::UIScrollLayout(const vec2& scroll_rect, bool horizontal) : scroll_rect(scroll_rect), UILayout(horizontal) {

    vertical_scrollbar   = new UIScrollBar(this, false);
    horizontal_scrollbar = new UIScrollBar(this, true);
    setFill(true);
}

UIScrollLayout::~UIScrollLayout() {
    delete vertical_scrollbar;
    delete horizontal_scrollbar;
}

vec2 UIScrollLayout::getRect() {
    return glm::min(scroll_rect,rect)+expanded_rect;
}

vec2 UIScrollLayout::getScrollRect() {
    return glm::min(scroll_rect,rect)+expanded_rect;
}

vec2 UIScrollLayout::getInnerRect() {
    return rect;
}

void UIScrollLayout::setUI(UI* ui) {
    UILayout::setUI(ui);

    vertical_scrollbar->setUI(ui);
    horizontal_scrollbar->setUI(ui);
}

void UIScrollLayout::drawBackground() {
    if(!drawbg) return;

    if(bgcolour.w > 0.0f) {
        glColor4fv(glm::value_ptr(bgcolour));
    } else {
        glColor4fv(glm::value_ptr(ui->getBackgroundColour()));
    }

    vec2 rect = glm::max(getRect(),getInnerRect());

    glDisable(GL_TEXTURE_2D);
    drawQuad(pos, rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glEnable(GL_TEXTURE_2D);
}

void UIScrollLayout::update(float dt) {

    if(fill_vertical && !parent) scroll_rect.y = display.height;

    UILayout::update(dt);

    //rect = scroll_rect;//glm::min( scroll_rect, rect );

    vertical_scrollbar->update(dt);
    horizontal_scrollbar->update(dt);
}

void UIScrollLayout::updatePos(const vec2& pos) {

    UILayout::updatePos(pos);

    vertical_scrollbar->updatePos();
    horizontal_scrollbar->updatePos();
}

void UIScrollLayout::scroll(bool up) {
    vertical_scrollbar->scroll(up);
}

UIElement* UIScrollLayout::elementAt(const vec2& pos) {

    if(vertical_scrollbar->elementAt(pos))   return vertical_scrollbar;
    if(horizontal_scrollbar->elementAt(pos)) return horizontal_scrollbar;

    if(!UIElement::elementAt(pos)) return 0;

    //apply scroll offset to position

    vec2 scrolled_pos = vec2(horizontal_scrollbar->bar_offset * rect.x, vertical_scrollbar->bar_offset * rect.y) + pos;

    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(scrolled_pos)) != 0) return found;
    }

    return this;
}

void UIScrollLayout::draw() {

    glEnable(GL_SCISSOR_TEST);

    vec2 scroll_offset = vec2(horizontal_scrollbar->bar_offset * -rect.x, vertical_scrollbar->bar_offset * -rect.y);

    glPushMatrix();
        glTranslatef(scroll_offset.x, scroll_offset.y, 0.0f);

        glScissor(pos.x, display.height-(pos.y+scroll_rect.y+expanded_rect.y), scroll_rect.x+expanded_rect.x, scroll_rect.y+expanded_rect.y);

        UILayout::draw();

        glDisable(GL_SCISSOR_TEST);
    glPopMatrix();

    vertical_scrollbar->draw();
    horizontal_scrollbar->draw();
}
