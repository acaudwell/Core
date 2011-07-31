#include "ui.h"
#include "element.h"

UI::UI() : selectedElement(0) {
    font = fontmanager.grab("FreeSans.ttf", 12);
    font.dropShadow(true);
}

UI::~UI() {
    clear();
}

void UI::clear() {
    foreach(UIElement* e, elements) {
        delete e;
    }
    elements.clear();
}

void UI::addElement(UIElement* e) {
    e->setUI(this);
    elements.push_back(e);
}

UIElement* UI::getSelected() {
    return selectedElement;
}

bool UI::elementsByType(std::list<UIElement*>& found, int type) {

    bool success = false;

    foreach(UIElement* e, elements) {
        if(e->elementsByType(found, type)) success = true;
    }

    return success;
}

UIElement* UI::elementAt(const vec2f& pos) {
    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) return found;
    }

    return 0;
}

UIElement* UI::selectElementAt(const vec2f& pos) {

    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) break;
    }

    if(selectedElement == found) return selectedElement;

    if(selectedElement != 0) {
        selectedElement->setSelected(false);
    }

    if(!found) {
        selectedElement  = 0;
        return 0;
    }

    selectedElement = found;

    selectedElement->setSelected(true);

    return selectedElement;
}

void UI::update(float dt) {

    foreach(UIElement* e, elements) {
        e->update(dt);
    }

    foreach(UIElement* e, elements) {
        e->updatePos(vec2f(0.0f, 0.0f));
    }
}



void UI::draw() {
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    foreach(UIElement* e, elements) {
        e->draw();
    }

    glDisable(GL_DEPTH_TEST);
}

void UI::drawOutline() {
    foreach(UIElement* e, elements) {
        e->drawOutline();
    }
}
