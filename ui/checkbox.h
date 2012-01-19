#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include "element.h"
#include "layout.h"
#include "label.h"

class UICheckbox : public UIElement {
protected:
    TextureResource* checktex;

public:
    UICheckbox();

    int getType() { return UI_CHECKBOX; };

    virtual void click(const vec2& pos) {};
    virtual bool isChecked() { return false; };
    void drawContent();
};

class UIBoolCheckbox : public UICheckbox {
protected:
    bool* value;
public:
    UIBoolCheckbox(bool* value);

    void click(const vec2& pos);
    bool isChecked();
};

class UIFloatCheckbox : public UICheckbox {
protected:
    float* value;
public:
    UIFloatCheckbox(float* value);

    void click(const vec2& pos);
    
    bool isChecked();
};

class UILabelBoolCheckbox : public UILayout {

public:
    UILabelBoolCheckbox(const std::string& label, bool* value);
};

class UILabelBoolCheckboxSet : public UILayout {

public:
    UILabelBoolCheckboxSet(const std::string& label, bool* value1, bool* value2, bool* value3);
};

class UILabelFloatCheckboxSet : public UILayout {

public:
    UILabelFloatCheckboxSet(const std::string& label, float* value1, float* value2, float* value3);
};

#endif
