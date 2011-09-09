#include "button.h"

UIButton::UIButton(const std::string& name, UIAction* action)
    : action(action), UISolidLayout(true) {

    label = new UILabel(name, false, 50.0f);
        
    addElement(label);

    button_anim = 0.0f;
    label->setMargin(2.0f);
}

void UIButton::click() {
    action->perform();

    inverted=true;
    button_anim = 0.25f;
    label->setMargin(vec4(2.0f, 3.0f, 2.0f, 1.0f));
}

void UIButton::update(float dt) {

    UILayout::update(dt);

    if(button_anim > 0.0f) {
        button_anim -=dt;
        if(button_anim <= 0.0f) {
            label->setMargin(2.0f);
            inverted=false;
        }
    }
}
