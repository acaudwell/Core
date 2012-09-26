#include "subgroup.h"
#include "image.h"

//UISubGroup

UISubGroup::UISubGroup(const std::string& groupname, bool minimized)
    : open_action(0), UILayout(false) {

    UISubGroupBar* bar = new UISubGroupBar(groupname);
    bar->parent = this;

    layout = new UILayout(false);
    layout->hidden = minimized;
    layout->setMargin(vec4(10.0f, 0.0f, 0.0f, 0.0f));
    layout->setFillHorizontal(true);

    addElement(bar);
    addElement(layout);

    setFillHorizontal(true);
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

bool UISubGroup::toggle() {
    layout->hidden = !layout->hidden;

    if(!layout->hidden && open_action != 0) {
        open_action->perform();
    }

    return layout->hidden;
}

//UISubGroupBar

UISubGroupBar::UISubGroupBar(const std::string& text) : UILayout(true) {
    selectable = true;

    expander = new UIImage("ui/expand.png", vec2(16.0f,16.0f), vec4(0.0f, 0.0f, 0.5f, 0.5f));

    addElement(new UILabel(text, false));
    addElement(expander);
}

void UISubGroupBar::click(const vec2& pos) {
    if(((UISubGroup*)parent)->toggle()) {
        expander->setTextureCoords(vec4(0.0f, 0.5f, 0.5f, 1.0f));
    } else {
        expander->setTextureCoords(vec4(0.0f, 0.0f, 0.5f, 0.5f));
    }
}
