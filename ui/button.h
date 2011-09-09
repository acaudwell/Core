#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "solidlayout.h"
#include "label.h"

class UIButton : public UISolidLayout {
    void (*action)();
    float button_anim;
    UILabel* label;
public:
    UIButton(const std::string& name, void (*action)());

    int getType() { return UI_BUTTON; }

    void click();

    void update(float dt);
};

#endif
