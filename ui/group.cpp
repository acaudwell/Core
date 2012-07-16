#include "group.h"

//UIGroup

UIGroup::UIGroup(const std::string& groupname, bool minimized, bool resizable)
    : minimized(minimized), UIElement() {

    selectable  = false;
    minimizable = true;

    bar    = new UIGroupBar(groupname);

    layout = resizable ? new UIResizableLayout() : new UILayout();

    bar->setMargin(3.0f);
    bar->parent = this;

    layout->setMargin(vec2(3.0f, 3.0f));
    layout->setPadding(3.0f);
    layout->setDrawBackground(true);
    layout->parent = this;

    animation = 0.0f;
    speed     = 2.5f;
}

UIGroup::~UIGroup() {
    if(bar != 0)    delete bar;
    if(layout != 0) delete layout;
}

void UIGroup::setUI(UI* ui) {
    this->ui = ui;
    bar->setUI(ui);
    layout->setUI(ui);
}

void UIGroup::setTitle(const std::string& text) {
    bar->setText(text);
}

void UIGroup::toggle() {
    if(!minimizable) return;

    minimized = !minimized;
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

void UIGroup::elementsAt(const vec2& pos, std::list<UIElement*>& elements_found) {

    if(hidden) return;

    bar->elementsAt(pos, elements_found);
    if(!minimized) layout->elementsAt(pos, elements_found);

    UIElement::elementsAt(pos, elements_found);
}

void UIGroup::updatePos(const vec2& pos) {
    this->pos = pos;

    bar->updatePos(pos);
    layout->updatePos(pos + vec2(0.0f, bar->rect.y));
}

void UIGroup::update(float dt) {

    bar->update(dt);

    rect = bar->rect;

    if(!minimized) {
        layout->update(dt);
        bar->rect.x =  std::max( bar->rect.x, layout->rect.x );
        rect = bar->rect;
        rect.y += layout->rect.y;
    } else {
    }

    if(animation>0.0f) {
        rect      = lerp2::lerp(old_group_rect, rect,    1.0f-animation);
        bar->rect = lerp2::lerp(old_label_rect, bar->rect, 1.0f-animation);

        animation -= dt*speed;
    }
}

void UIGroup::draw() {
    if(hidden) return;
    bar->draw();
    if(!minimized && animation <= 0.0f) layout->draw();
}

//UIGroupBar

UIGroupBar::UIGroupBar(const std::string& text) : UISolidLayout(true) {
    label = new UILabel(text, false, 120.0f);
    addElement(label);
    selectable = true;
}

void UIGroupBar::setText(const std::string& text) {
    label->setText(text);
}

void UIGroupBar::click(const vec2& pos) {
    ((UIGroup*)parent)->toggle();
}
