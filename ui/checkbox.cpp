#include "checkbox.h"

//UICheckbox

UICheckbox::UICheckbox() : UIElement() {
    checktex = texturemanager.grab("ui/checkbox.png");
    rect = vec2(16, 16.0f);
}

void UICheckbox::drawContent() {
    checktex->bind();

    //background
    if(isChecked()) {
        drawQuad(vec2(16.0f,16.0f), vec4(0.0f, 0.5f, 0.5f, 1.0f));
    } else {
        drawQuad(vec2(16.0f,16.0f), vec4(0.0f, 0.0f, 0.5f, 0.5f));
    }
}

//UIBoolCheckbox

UIBoolCheckbox::UIBoolCheckbox(bool *value) : value(value), UICheckbox() {   
}

void UIBoolCheckbox::click(const vec2& pos) {
    *value = !(*value);
}

bool UIBoolCheckbox::isChecked() {
    return *value;
}

//UIFloatCheckbox

UIFloatCheckbox::UIFloatCheckbox(float *value) : value(value), UICheckbox() {   
}

void UIFloatCheckbox::click(const vec2& pos) {
    *value = isChecked() ? 0.0f : 1.0f; 
}

bool UIFloatCheckbox::isChecked() {
    return *value == 1.0f;
}

//UILabelCheckbox

UILabelBoolCheckbox::UILabelBoolCheckbox(const std::string& label, bool* value) : UILayout(true) {

    addElement(new UILabel(label, false, 120.0f));
    addElement(new UIBoolCheckbox(value));

    padding = vec2(2.0f);
}

UILabelBoolCheckboxSet::UILabelBoolCheckboxSet(const std::string& label, bool* value1, bool* value2, bool* value3) : UILayout(true) {

    addElement(new UILabel(label, false, 120.0f));
    addElement(new UIBoolCheckbox(value1));
    addElement(new UIBoolCheckbox(value2));
    addElement(new UIBoolCheckbox(value3));

    padding = vec2(2.0f);
}

UILabelFloatCheckboxSet::UILabelFloatCheckboxSet(const std::string& label, float* value1, float* value2, float* value3) : UILayout(true) {

    addElement(new UILabel(label, false, 120.0f));
    addElement(new UIFloatCheckbox(value1));
    addElement(new UIFloatCheckbox(value2));
    addElement(new UIFloatCheckbox(value3));

    padding = vec2(2.0f);
}
