#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "solidlayout.h"
#include "label.h"
#include "action.h"

class UIButton : public UISolidLayout {
    float button_anim;
    UILabel* label;
    UIAction* action;
public:
    UIButton(const std::string& name, UIAction* action);

    int getType() { return UI_BUTTON; }

    void click(const vec2& pos);

    void update(float dt);
};

#endif
