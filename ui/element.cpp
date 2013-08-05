#include "element.h"

#include <boost/assign/list_of.hpp>

std::map<int,std::string> element_names = boost::assign::map_list_of
    (UI_INVALID,    "Invalid"   )
    (UI_ELEMENT,    "Element"   )
    (UI_LABEL,      "Label"     )
    (UI_BUTTON,     "Button"    )
    (UI_IMAGE,      "Image"     )
    (UI_LAYOUT,     "Layout"    )
    (UI_GROUP,      "Group"     )
    (UI_COLOUR,     "Colour"    )
    (UI_SELECT,     "Select"    )
    (UI_SLIDER,     "Slider"    )
    (UI_SCROLL_BAR, "ScrollBar" )
    (UI_CHECKBOX,   "Checkbox"  );

UIElement::~UIElement() {
    { if(ui && selected) ui->deselect(); };
}

const std::string& UIElement::getElementName() const {
    return getElementName(this->getType());
}

const std::string& UIElement::getElementName(int type) {

    auto it = element_names.find(type);

    if(it != element_names.end()) return it->second;

    return element_names[UI_INVALID];
}

void UIElement::drawOutline() {
    //fprintf(stderr, "rect %.2f, %.2f\n", rect.x, rect.y);
    ui->setTextured(false);

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glPushMatrix();
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glLineWidth(1.0f);
        glTranslatef(0.5f, 0.5f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(rect.x, 0.0f);
            glVertex2f(rect.x, rect.y);
            glVertex2f(0.0f, rect.y);
        glEnd();

    glPopMatrix();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

     ui->setTextured(true);
}

void UIElement::getModifiers(bool& left_ctrl, bool& left_shift) {

    left_ctrl = left_shift = false;

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_LCTRL])  left_ctrl  = true;
    if(keystate[SDL_SCANCODE_LSHIFT]) left_shift = true;
}

double UIElement::granulaity(double initial, double scale) {

    bool left_ctrl, left_shift;
    getModifiers(left_ctrl, left_shift);

    double granul = initial;

    if(left_ctrl)  granul *= scale;
    if(left_shift) granul *= scale;

    return granul;
}

void UIElement::drawOutline(const vec2& rect) {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glPushMatrix();
        glLineWidth(1.0f);
        glTranslatef(0.5f, 0.5f, 0.0f);

        glBegin(GL_QUADS);
            glVertex2f(0.0f,    0.0f);
            glVertex2f(rect.x,  0.0f);
            glVertex2f(rect.x, rect.y);
            glVertex2f(0.0f,   rect.y);
        glEnd();

    glPopMatrix();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void UIElement::drawQuad(const vec2& pos, const vec2& rect, const vec4& texcoord) {

    glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0f);
        drawQuad(rect, texcoord);
    glPopMatrix();
}

void UIElement::drawQuad(const vec2& rect, const vec4& texcoord) {

    glBegin(GL_QUADS);
        glTexCoord2f(texcoord.x,texcoord.y);
        glVertex2f(0.0f,    0.0f);

        glTexCoord2f(texcoord.z,texcoord.y);
        glVertex2f(rect.x,  0.0f);

        glTexCoord2f(texcoord.z,texcoord.w);
        glVertex2f(rect.x, rect.y);

        glTexCoord2f(texcoord.x,texcoord.w);
        glVertex2f(0.0f,   rect.y);
    glEnd();
}

void UIElement::draw() {
    glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0f);
        drawContent();
        //if(selected) drawOutline();
    glPopMatrix();
}

void UIElement::update(float dt) {
    updateContent();
    updateRect();
    updateZIndex();
}

void UIElement::updateZIndex() {
    if(parent != 0) zindex = parent->zindex + 1;
}

void UIElement::scroll(bool up) {
    if(parent != 0) parent->scroll(up);
}

bool UIElement::elementsByType(std::list<UIElement*>& found, int type) {

    if(getType() == type) {
        found.push_back(this);
        return true;
    }

    return false;
}

void UIElement::elementsAt(const vec2& pos, std::list<UIElement*>& elements_found) {

    if(hidden) return;

    vec2 rect = getRect();

    if(   pos.x >= this->pos.x && pos.x <= (this->pos.x + rect.x)
       && pos.y >= this->pos.y && pos.y <= (this->pos.y + rect.y)) {
        elements_found.push_back(this);
    }
}
