#include "element.h"

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

#if SDL_VERSION_ATLEAST(1,3,0)
    Uint8* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_LCTRL])  left_ctrl  = true;
    if(keystate[SDL_SCANCODE_LSHIFT]) left_shift = true;
#else
    Uint8* keystate = SDL_GetKeyState(NULL);

    if(keystate[SDLK_LCTRL])  left_ctrl  = true;
    if(keystate[SDLK_LSHIFT]) left_shift = true;
#endif
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

UIElement* UIElement::elementAt(const vec2& pos) {

    if(hidden) return 0;

    vec2 rect = getRect();

    if(   pos.x >= this->pos.x && pos.x <= (this->pos.x + rect.x)
       && pos.y >= this->pos.y && pos.y <= (this->pos.y + rect.y)) {
        return this;
    }
    return 0;
}
