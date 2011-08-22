#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "label.h"

class UIButton : public UILabel {
    void (*action)();
public:
    UIButton(const std::string& name, void (*action)());

    int getType() { return UI_BUTTON; }

    void click();
};

#endif