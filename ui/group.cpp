#include "group.h"

//UIGroup

UIGroup::UIGroup(const std::string& groupname, bool open) : open(open) {
    bar    = new UIGroupBar(groupname);
    layout = new UILayout();
   
    bar->setMargin(5.0f);
    bar->parent = this;

    layout->bgcolour = vec4(0.3f, 0.3f, 0.3f, 0.67f);
    layout->setMargin(vec2(5.0f, 5.0f));
    layout->setPadding(5.0f);

    animation = 0.0f;
    speed     = 2.5f;
}

UIGroup::~UIGroup() {
    if(bar != 0)    delete bar;
    if(layout != 0) delete layout;
}

void UIGroup::setUI(UI* ui) {
    bar->setUI(ui);
    layout->setUI(ui);
}

void UIGroup::setTitle(const std::string& text) {
    bar->setText(text);
}

void UIGroup::click() {
    open = !open;
    animation = 1.0f;

    old_group_rect = rect;
    old_label_rect = bar->rect;
}

bool UIGroup::elementsByType(std::list<UIElement*>& found, int type) {

    bool success = UIElement::elementsByType(found, type);

    if(bar->elementsByType(found, type))  success = true;
    if(layout->elementsByType(found, type)) success = true;

    return success;
}

UIElement* UIGroup::elementAt(const vec2& pos) {

    UIElement* found = 0;

    if((found = bar->elementAt(pos)) != 0) return found;
    if(open && (found = layout->elementAt(pos)) != 0) return found;

    return UIElement::elementAt(pos);
}

void UIGroup::updatePos(const vec2& pos) {
    this->pos = pos;

    bar->updatePos(pos);
    layout->updatePos(pos + vec2(0.0f, bar->rect.y));
}

void UIGroup::update(float dt) {

    bar->update(dt);

    rect = bar->rect;

    if(open) {
        layout->update(dt);
        bar->rect.x =  std::max( bar->rect.x, layout->rect.x );
        rect = bar->rect;
        rect.y += layout->rect.y;
    }

    if(animation>0.0f) {
        rect      = lerp2::lerp(old_group_rect, rect,    1.0f-animation);
        bar->rect = lerp2::lerp(old_label_rect, bar->rect, 1.0f-animation);

        animation -= dt*speed;
    }
}

void UIGroup::draw() {
    bar->draw();
    if(open && animation <= 0.0f) layout->draw();
}

//UIGroupBar

UIGroupBar::UIGroupBar(const std::string& text) : UISolidLayout(true) {
    label = new UILabel(text, false, 120.0f);
    addElement(label);
}

void UIGroupBar::setText(const std::string& text) {
    label->setText(text);
}

void UIGroupBar::click() {
    parent->click();
}

