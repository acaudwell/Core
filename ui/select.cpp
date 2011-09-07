#include "select.h"

UISelect::UISelect() : UILabel("blah", false, true) {
    options_layout = new UILayout();
    options_layout->parent = this;
    
    setMargin(vec4(2.0f,2.0f,2.0f,2.0f));
    selected_option = -1;
    width=150.0f;
}

void UISelect::setUI(UI* ui) {
    this->ui = ui;
    options_layout->setUI(ui);
}

UISelect::~UISelect() {
    delete options_layout;
}

UIElement* UISelect::elementAt(const vec2& pos) {
    return UIElement::elementAt(pos);
}

void UISelect::selectOption(int index) {
    if(index<0 || index>=options.size()) return;

    setText(options[index].first);
    selected_option = index;
}

void UISelect::addOption(const std::string& name, const std::string& value) {
    options_layout->addElement(new UILabel(name, false, true));
    options.push_back(std::pair<std::string,std::string>(name,value));
    
    if(options.size()==1) selectOption(0);
}
