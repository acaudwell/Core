#include "ui.h"
#include "checkbox.h"
#include "group.h"
#include "slider.h"
#include "scroll_bar.h"
#include "select.h"
#include "button.h"
#include "colour.h"

UI::UI() : selectedElement(0) {
    font = fontmanager.grab("FreeSans.ttf", 12);
    font.dropShadow(true);
    double_click_interval = 0.5f;
    double_click_timer = double_click_interval;
    interaction = false;

    background_colour = vec4(0.3f, 0.3f, 0.3f, 0.67f);
    solid_colour      = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    text_colour       = vec4(1.0f);
    ui_alpha          = vec4(1.0f);
}

UI::~UI() {
    clear();
}

void UI::clear() {
    deselect();
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

vec4 UI::getBackgroundColour() {
    return background_colour * ui_alpha;
}

vec4 UI::getSolidColour() {
    return solid_colour * ui_alpha;
}

vec4 UI::getTextColour() {
    return text_colour * ui_alpha;
}

vec4 UI::getAlpha() {
    return ui_alpha;
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

void UI::selectElement(UIElement* element) {
    if(selectedElement != 0) {
        selectedElement->setSelected(false);
    }
    selectedElement = element;

    if(element!=0) {
        element->setSelected(true);
    }
}

UIElement* UI::selectElementAt(const vec2& pos) {

    double_click_timer = 0.0f;

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

    if(ui_alpha.w < 1.0f) {
        ui_alpha.w = glm::min(ui_alpha.w + dt * 0.5f, 1.0f);
    }

    if(!interaction) {
        UIElement* selected = getSelected();

        if(selected) selected->idle();
    }

    interaction = false;

    if(double_click_timer<double_click_interval) double_click_timer += dt;

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
        return selected->isEditable();
    }

    char c = toChar(e);

    if(!c) return false;

    return selected->keyPress(e, c);
}

void UI::drag(const MouseCursor& cursor) {

    interaction = true;

    UIElement* selected = getSelected();

    if(!selected) return;

    selected->drag(cursor.getPos());
}

UIElement* UI::click(const MouseCursor& cursor) {

    interaction = true;

    UIElement* previous = getSelected();
    bool double_click   = double_click_timer < 0.5f;

    vec2 pos = cursor.getPos();

    UIElement* selected = selectElementAt(pos);

    if(!selected) return 0;

    if(previous == selected && double_click) {
        selected->doubleClick(pos);
    } else {
        selected->click(pos);
    }

    return selected;
}

UIColour* UI::getActiveColour() {

    std::list<UIElement*> found;
    elementsByType(found, UI_COLOUR);

    foreach(UIElement* e, found) {
        UIColour* c = (UIColour*)e;

        if(c->active) return c;
    }

    return 0;
}
