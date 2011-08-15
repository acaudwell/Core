#include "group.h"

//UIGroup

UIGroup::UIGroup(const std::string& groupname, bool open) : open(open) {
    label  = new UILabel(groupname, false, true);
    layout = new UILayout();

    label->setMargin(5.0f);
    label->parent = this;

    layout->setMargin(vec2(0.0f, 5.0f));
    layout->setPadding(5.0f);
    
    animation = 0.0f;
    speed     = 2.5f;
}

UIGroup::~UIGroup() {
    if(label != 0)  delete label;
    if(layout != 0) delete layout;
}

void UIGroup::setUI(UI* ui) {
    label->setUI(ui);
    layout->setUI(ui);
}

void UIGroup::setTitle(const std::string& text) {
    label->setText(text);
}

void UIGroup::toggle() {
    open = !open;
    animation = 1.0f;
    
    old_group_rect = rect;
    old_label_rect = label->rect;
}

bool UIGroup::elementsByType(std::list<UIElement*>& found, int type) {

    bool success = UIElement::elementsByType(found, type);

    if(label->elementsByType(found, type))  success = true;
    if(layout->elementsByType(found, type)) success = true;

    return success;
}

UIElement* UIGroup::elementAt(const vec2& pos) {

    if(!UIElement::elementAt(pos)) return 0;

    UIElement* found = 0;

    if((found = label->elementAt(pos)) != 0) return found;
    if(open && (found = layout->elementAt(pos)) != 0) return found;

    return 0;
}

void UIGroup::updatePos(const vec2& pos) {
    this->pos = pos;

    label->updatePos(pos);
    layout->updatePos(pos + vec2(0.0f, label->rect.y));
}

void UIGroup::update(float dt) {

    label->update(dt);

    rect = label->rect;

    if(open) {
        layout->update(dt);
        label->rect.x =  std::max( label->rect.x, layout->rect.x );
        rect = label->rect;
        rect.y += layout->rect.y;
    }

    if(animation>0.0f) {
        rect        = lerp2::lerp(old_group_rect, rect,        1.0f-animation);
        label->rect = lerp2::lerp(old_label_rect, label->rect, 1.0f-animation);
        
        animation -= dt*speed;
    }
}

void UIGroup::draw() {
    label->draw();
    if(open && animation <= 0.0f) layout->draw();
}

