#include "subgroup.h"

//UISubGroup

UISubGroup::UISubGroup(const std::string& groupname, bool minimized)
    : open_action(0), UILayout(false) {

    UISubGroupBar* bar = new UISubGroupBar(groupname);
    bar->parent = this;

    layout = new UILayout(false);
    layout->hidden = minimized;
    layout->setMargin(vec4(10.0f, 0.0f, 0.0f, 0.0f));

    addElement(bar);
    addElement(layout);
}


void UISubGroup::setOpenAction(UIAction* action) {
    this->open_action = action;
}

void UISubGroup::minimize() {
    layout->hidden = true;
}

void UISubGroup::maximize() {
    layout->hidden = false;
}

void UISubGroup::toggle() {
    layout->hidden = !layout->hidden;

    if(!layout->hidden && open_action != 0) {
        open_action->perform();
    }
}

//UISubGroupBar

UISubGroupBar::UISubGroupBar(const std::string& text) : UILayout() {
    selectable = true;
    addElement(new UILabel(text, false, 120.0f));
}

void UISubGroupBar::click(const vec2& pos) {
    ((UISubGroup*)parent)->toggle();
}
