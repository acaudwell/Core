#include "group.h"

//UIGroup

UIGroup::UIGroup(const std::string& groupname, bool minimized, bool resizable)
    : UILayout(false) {

    selectable  = false;
    minimizable = true;

    layout = resizable ? new UIResizableLayout(): new UILayout();
    layout->hidden = minimized;

    bar = new UIGroupBar(groupname);
    bar->setMargin(3.0f);

    layout->setMargin(vec2(3.0f, 3.0f));
    layout->setPadding(3.0f);
    layout->setDrawBackground(true);
    layout->setFillHorizontal(true);
    layout->parent = this;

    setMinRect(vec2(350.0f, 0.0f));

    addElement(bar);
    addElement(layout);

    open_action = 0;
}

void UIGroup::setTitle(const std::string& text) {
    bar->setText(text);
}

void UIGroup::minimize() {
    if(layout->hidden) return;
    layout->hidden = true;
}

void UIGroup::maximize() {
    layout->hidden=false;
}

void UIGroup::toggle() {
    if(!minimizable) return;

    layout->hidden = !layout->hidden;

    if(!layout->hidden && open_action != 0) {
        open_action->perform();
    }
}

void UIGroup::setOpenAction(UIAction* action) {
    open_action = action;
}

//UIGroupBar

UIGroupBar::UIGroupBar(const std::string& text) : UISolidLayout(true) {
    selectable = true;

    label = new UILabel(text, false);
    addElement(label);

    setFillHorizontal(true);
}

void UIGroupBar::setText(const std::string& text) {
    label->setText(text);
}

void UIGroupBar::click(const vec2& pos) {
    ((UIGroup*)parent)->toggle();
}
