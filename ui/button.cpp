#include "button.h"

UIButton::UIButton(const std::string& name, UIAction* action)
    : action(action), UISolidLayout(false) {

    label = new UILabel(name, false);

    addElement(label);

    button_anim = 0.0f;
    label->setMargin(3.0f);

    selectable = true;
    centre = true;
    min_rect = vec2(50.0f, 1.0f);
}

void UIButton::click(const vec2& pos) {
    action->perform();

    inverted=true;
    button_anim = 0.25f;
}

void UIButton::update(float dt) {

    UILayout::update(dt);

    if(button_anim > 0.0f) {
        button_anim -=dt;
        if(button_anim <= 0.0f) inverted=false;
    }
}
