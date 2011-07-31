#include "element.h"

void UIElement::drawOutline() {
    //fprintf(stderr, "rect %.2f, %.2f\n", rect.x, rect.y);
    glDisable(GL_TEXTURE_2D);
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

    glEnable(GL_TEXTURE_2D);
}

void UIElement::drawOutline(const vec2f& rect) {
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

void UIElement::drawQuad(const vec2f& pos, const vec2f& rect, const vec4f& texcoord) {

    glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0f);
        drawQuad(rect, texcoord);
    glPopMatrix();
}

void UIElement::drawQuad(const vec2f& rect, const vec4f& texcoord) {
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
    updateRect();
}

bool UIElement::elementsByType(std::list<UIElement*>& found, int type) {

    if(getType() == type) {
        found.push_back(this);
        return true;
    }

    return false;
}

UIElement* UIElement::elementAt(const vec2f& pos) {
    if(   pos.x >= this->pos.x && pos.x <= (this->pos.x + rect.x)
       && pos.y >= this->pos.y && pos.y <= (this->pos.y + rect.y)) {
        return this;
    }
    return 0;
}
