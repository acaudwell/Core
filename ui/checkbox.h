#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include "element.h"
#include "layout.h"
#include "label.h"

class UICheckbox : public UIElement {
protected:
    TextureResource* checktex;
    UIAction* action;
public:
    UICheckbox(UIAction* action = 0);

    int getType() const { return UI_CHECKBOX; };

    virtual void click(const vec2& pos) {};
    virtual bool isChecked() { return false; };
    void drawContent();
    void idle();
};

class UIBoolCheckbox : public UICheckbox {
protected:
    bool* value;
public:
    UIBoolCheckbox(bool* value, UIAction* action = 0);

    void click(const vec2& pos);
    bool isChecked();
};

class UIFloatCheckbox : public UICheckbox {
protected:
    float* value;
public:
    UIFloatCheckbox(float* value, UIAction* action = 0);

    void click(const vec2& pos);

    bool isChecked();
};

class UILabelBoolCheckbox : public UILabelledElement {

public:
    UILabelBoolCheckbox(const std::string& label, bool* value,UIAction* action = 0);
};

class UILabelBoolCheckboxSet : public UILabelledElement {

public:
    UILabelBoolCheckboxSet(const std::string& label, bool* value1, bool* value2, bool* value3, UIAction* action = 0);
};

class UILabelFloatCheckboxSet : public UILabelledElement {

public:
    UILabelFloatCheckboxSet(const std::string& label, float* value1, float* value2, float* value3, UIAction* action = 0);
};

#endif
