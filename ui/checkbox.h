#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include "element.h"
#include "layout.h"
#include "label.h"

class UICheckbox : public UIElement {
protected:
    TextureResource* checktex;

    bool* value;
public:
    UICheckbox(bool* value);

    int getType() { return UI_CHECKBOX; };

    void toggle();

    void drawContent();
};


class UILabelCheckbox : public UILayout {

public:
    UILabelCheckbox(const std::string& label, bool* value);
};

#endif
