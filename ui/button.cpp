#include "button.h"

UIButton::UIButton(const std::string& name, void (*action)())
    : action(action), UILabel(name, false, true) {
    width = 50.0f;
    setMargin(2.0f);
}

void UIButton::click() {
    void (*action)();
}
