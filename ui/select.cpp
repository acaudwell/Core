#include "select.h"

UISelect::UISelect() : UISolidLayout(true) {

    label = new UILabel("Select", false, 150.0f);   
    addElement(label);

    options_layout = new UISolidLayout(false);
    options_layout->setMargin(2.0f);
    options_layout->parent = this;
    
    setMargin(vec4(2.0f,2.0f,2.0f,2.0f));
    selected_option = 0;
    
    open = false;
}

void UISelect::setUI(UI* ui) {
    UISolidLayout::setUI(ui);
    options_layout->setUI(ui);
}

UISelect::~UISelect() {
    delete options_layout;
}

void UISelect::click() {
    open = !open;
}

UIOptionLabel* UISelect::getSelectedOption() {
    return selected_option;
}

void UISelect::selectOption(UIOptionLabel* option) {
    label->setText(option->text);
    selected_option = option;
    open = false;
}

void UISelect::addOption(const std::string& name, const std::string& value) {
    UIOptionLabel* option = new UIOptionLabel(this, name, value);
    
    options_layout->addElement(option);   

    selectOption(option);
}

UIElement* UISelect::elementAt(const vec2& pos) {

    UIElement* found = 0;

    if(open && (found = options_layout->elementAt(pos)) != 0) {
        return found;
    }

    return UISolidLayout::elementAt(pos);
}

void UISelect::updatePos(const vec2& pos) {
    UISolidLayout::updatePos(pos);
    options_layout->updatePos(pos);
}

void UISelect::update(float dt) {
    UISolidLayout::update(dt);
    
    options_layout->update(dt);

}

void UISelect::draw() {
    if(!open) UISolidLayout::draw();
    else options_layout->draw();
}

UIOptionLabel::UIOptionLabel(UISelect* select, const std::string& text, const std::string& value)
    : select(select), value(value), UILabel(text, false, 150.0f) {
}

void UIOptionLabel::click() {
    select->selectOption(this);
}