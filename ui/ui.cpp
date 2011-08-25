#include "ui.h"
#include "checkbox.h"
#include "group.h"
#include "slider.h"
#include "button.h"

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

UIElement* UI::elementAt(const vec2& pos) {
    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) return found;
    }

    return 0;
}

void UI::deselect() {
    if(!selectedElement) return;

    selectedElement->setSelected(false);
    selectedElement = 0;
}

UIElement* UI::selectElementAt(const vec2& pos) {

    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) break;
    }

    if(selectedElement == found) return selectedElement;

    if(selectedElement != 0) {
        selectedElement->setSelected(false);
    }

    if(!found) {
        selectedElement = 0;
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
        e->updatePos(vec2(0.0f, 0.0f));
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

char UI::toChar(SDL_KeyboardEvent *e) {

    int unicode = e->keysym.unicode;

    if( unicode > 0x80 && unicode <= 0 ) return 0;

    char c         = unicode;
    bool uppercase = SDL_GetModState() & KMOD_SHIFT;

    if(uppercase) {
        return toupper(c);
    }

    return c;
}

bool UI::keyPress(SDL_KeyboardEvent *e) {

    UIElement* selected = getSelected();

    if(!selected) return false;

    if(e->keysym.unicode == SDLK_ESCAPE) {
        deselect();
        return true;
    }

    char c = toChar(e);

    if(!c) return false;

    return selected->keyPress(e, c);
}

void UI::processMouse(const MouseCursor& cursor, bool left_click, bool left_down) {

    UIElement* selected = getSelected();

    if(!selected) return;

    if(selected->getType() == UI_BUTTON && left_click) {
        ((UIButton*)selected)->click();
    }

    if(selected->getType() == UI_CHECKBOX && left_click) {
        ((UICheckbox*)selected)->toggle();
    }

    if(selected->getType() == UI_LABEL && (selected->parent != 0) && (selected->parent->getType() == UI_GROUP) && left_click) {
        ((UIGroup*)selected->parent)->toggle();
    }

    if(selected->getType() == UI_SLIDER && left_down) {
        ((UISlider*)selected)->selectValueAt(cursor.getPos());
    }
}
