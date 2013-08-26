#include "select.h"

UISelect::UISelect() : UISolidLayout(true) {

    label = new UILabel("Select", false, 150.0f);
    addElement(label);

    options_layout = new UISolidLayout(false);
    options_layout->setMargin(3.0f);
    options_layout->parent = this;

    setMargin(vec4(3.0f));
    selected_option = 0;

    open = false;
    selectable = true;

    selectex = texturemanager.grab("ui/select.png", false);
    selectex->bind();
    selectex->setFiltering(GL_NEAREST, GL_NEAREST);
    selectex->setWrapStyle(GL_CLAMP);
}

void UISelect::setUI(UI* ui) {
    UISolidLayout::setUI(ui);
    options_layout->setUI(ui);
}

UISelect::~UISelect() {
    delete options_layout;
    if(selectex!=0) texturemanager.release(selectex);
}

void UISelect::click(const vec2& pos) {
    open = !open;
}

UIOptionLabel* UISelect::getSelectedOption() {
    return selected_option;
}

void UISelect::selectOption(UIOptionLabel* option, bool submit) {
    label->setText(option->text);
    selected_option = option;
    if(submit) selected_option->submit();
    open = false;
}

UIOptionLabel* UISelect::addOption(const std::string& name, const std::string& value, bool select_option) {
    UIOptionLabel* option = new UIOptionLabel(this, name, value);

    options_layout->addElement(option);

    if(select_option) selectOption(option);

    // if we have no selected option select option but dont submit
    if(!selected_option) selectOption(option, false);

    return option;
}

UIOptionLabel* UISelect::addOption(const std::string& name, UIAction* action, bool select_option) {
    UIOptionLabel* option = new UIOptionLabel(this, name, action);

    options_layout->addElement(option);

    if(select_option) selectOption(option);

    // if we have no selected option select option but dont submit
    if(!selected_option) selectOption(option, false);

    return option;
}

void UISelect::elementsAt(const vec2& pos, std::list<UIElement*>& elements_found) {

    UIElement* found = 0;

    if(open) {
        options_layout->elementsAt(pos, elements_found);
    }

    UISolidLayout::elementsAt(pos, elements_found);
}

void UISelect::updatePos(const vec2& pos) {

    vec2 adjusted_pos = pos;

    if(open && parent && adjusted_pos.y + options_layout->getRect().y > parent->getRect().y) {
        adjusted_pos.y -= options_layout->getRect().y - this->getRect().y;
    }

    UISolidLayout::updatePos(adjusted_pos);
    options_layout->updatePos(adjusted_pos);
}

void UISelect::update(float dt) {

    updateZIndex();

    if(open) options_layout->zindex = this->zindex + 1;
        else options_layout->zindex = this->zindex;

    UISolidLayout::update(dt);
    options_layout->update(dt);
}

void UISelect::draw() {
    if(!open) {
        UISolidLayout::draw();

        selectex->bind();
        glColor4fv(glm::value_ptr(ui->getSolidColour()));
        drawQuad(pos + vec2(rect.x-16.0f,1.0f), vec2(16.0f, 16.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }
    else {
        options_layout->draw();
    }
}

UIIntSelectAction::UIIntSelectAction(int* field, int value, UIAction* action) : field(field), value(value), action(action) {
}

void UIIntSelectAction::perform() {
    *field = value;
    if(action != 0) {
        action->perform();
    }
}

// UIOptionLabel

UIOptionLabel::UIOptionLabel(UISelect* select, const std::string& text, const std::string& value, UIAction* action)
    : select(select), value(value), action(action), UILabel(text, false, 150.0f) {
    selectable = true;
}

UIOptionLabel::UIOptionLabel(UISelect* select, const std::string& text, UIAction* action)
    : select(select), value(text), action(action), UILabel(text, false, 150.0f) {
    selectable = true;
}

bool UIOptionLabel::submit() {

    if(action!=0) {
        action->perform();
        return true;
    }

    return false;
}

void UIOptionLabel::click(const vec2& pos) {
    select->selectOption(this);
}
