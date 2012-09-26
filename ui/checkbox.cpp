#include "checkbox.h"

//UICheckbox

UICheckbox::UICheckbox(UIAction* action)
    : action(action), UIElement() {
    checktex = texturemanager.grab("ui/checkbox.png");
    rect = vec2(16, 16.0f);
}

void UICheckbox::drawContent() {
    checktex->bind();

    glColor4fv(glm::value_ptr(ui->getTintColour()));

    //background
    if(isChecked()) {
        drawQuad(vec2(16.0f,16.0f), vec4(0.0f, 0.5f, 0.5f, 1.0f));
    } else {
        drawQuad(vec2(16.0f,16.0f), vec4(0.0f, 0.0f, 0.5f, 0.5f));
    }
}

void UICheckbox::idle() {
    if(action != 0) action->idle();
}

//UIBoolCheckbox

UIBoolCheckbox::UIBoolCheckbox(bool *value, UIAction* action)
    : value(value), UICheckbox(action) {
}

void UIBoolCheckbox::click(const vec2& pos) {
    *value = !(*value);
    if(action != 0) action->perform();
}

bool UIBoolCheckbox::isChecked() {
    return *value;
}

//UIFloatCheckbox

UIFloatCheckbox::UIFloatCheckbox(float *value, UIAction* action)
    : value(value), UICheckbox(action) {
}

void UIFloatCheckbox::click(const vec2& pos) {
    *value = isChecked() ? 0.0f : 1.0f;
    if(action != 0) action->perform();
}

bool UIFloatCheckbox::isChecked() {
    return *value == 1.0f;
}

//UILabelBoolCheckbox

UILabelBoolCheckbox::UILabelBoolCheckbox(const std::string& label, bool* value, UIAction* action) : UILabelledElement(label) {

    layout->setMinRect(vec2(216.0f, 0.0f));
    layout->addElement(new UIBoolCheckbox(value, action));
    layout->setPadding(2.0f);
}

//UILabelBoolCheckboxSet

UILabelBoolCheckboxSet::UILabelBoolCheckboxSet(const std::string& label, bool* value1, bool* value2, bool* value3, UIAction* action) : UILabelledElement(label) {

    layout->setMinRect(vec2(216.0f, 0.0f));
    layout->addElement(new UIBoolCheckbox(value1, action));
    layout->addElement(new UIBoolCheckbox(value2, action));
    layout->addElement(new UIBoolCheckbox(value3, action));
    layout->setPadding(2.0f);
}

//UILabelFloatCheckboxSet

UILabelFloatCheckboxSet::UILabelFloatCheckboxSet(const std::string& label, float* value1, float* value2, float* value3, UIAction* action) : UILabelledElement(label) {

    layout->setMinRect(vec2(216.0f, 0.0f));
    layout->addElement(new UIFloatCheckbox(value1, action));
    layout->addElement(new UIFloatCheckbox(value2, action));
    layout->addElement(new UIFloatCheckbox(value3, action));
    layout->setPadding(2.0f);
}
