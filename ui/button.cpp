#include "button.h"

UIButton::UIButton(const std::string& name, void (*action)())
    : action(action), UILabel(name, false, true) {
    width = 50.0f;
    button_anim = 0.0f;
    setMargin(2.0f);
}

void UIButton::click() {
    action();
    inverted=true;
    button_anim = 0.25f;
    setMargin(vec4(2.0f, 3.0f, 2.0f, 1.0f));
}

void UIButton::update(float dt) {

    UILabel::update(dt);

    if(button_anim > 0.0f) {
        button_anim -=dt;
        if(button_anim <= 0.0f) {
            setMargin(2.0f);
            inverted=false;
        }
    }
}
