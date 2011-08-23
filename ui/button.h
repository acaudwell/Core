#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "label.h"

class UIButton : public UILabel {
    void (*action)();
    float button_anim;
public:
    UIButton(const std::string& name, void (*action)());

    int getType() { return UI_BUTTON; }

    void click();

    void update(float dt);
};

#endif
