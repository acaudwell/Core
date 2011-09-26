#include "slider.h"

//UISlider

UISlider::UISlider(const std::string& slider_texture, float width) : slider_width(width), UIElement() {
    slidertex  = texturemanager.grab(slider_texture);
    rect       = vec2(slider_width, 16.0f);
    background = true;
}

void UISlider::drawSlider(float position) {

    float slider_position = position * (slider_width-4.0f) + 4.0f;

    //background
    //drawQuad(vec2(slider_width,16.0f), vec4(0.0f, 0.0f, 1.0f, 0.5f));

    if(background) {

        glDisable(GL_TEXTURE_2D);

        glLineWidth(1.0f);

        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 3.0f);
            glVertex2f(slider_width, 3.0f);
            glVertex2f(slider_width, 11.0f);
            glVertex2f(0.0f, 11.0f);
        glEnd();


        glPushMatrix();
            glTranslatef(0.5f, 0.5f, 0.0f);

            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            glBegin(GL_LINE_STRIP);
                glVertex2f(0.0f, 3.0f);
                glVertex2f(slider_width, 3.0f);
                glVertex2f(slider_width, 11.0f);
                glVertex2f(0.0f, 11.0f);
                glVertex2f(0.0f, 3.0f);
            glEnd();

        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
    }

    slidertex->bind();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    //slider
    drawQuad(vec2(slider_position-4.0, 0.0f), vec2(16.0,16.0), vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

//UIFloatSlider

UIFloatSlider::UIFloatSlider(float* value, float min, float max) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f) {
}

void UIFloatSlider::scroll(bool up) {

    float value_inc = (max-min) / 100.0f;

    if(!up) value_inc = -value_inc;

    Uint8* keyState = SDL_GetKeyState(NULL);
    if(keyState[SDLK_LCTRL]) {
        value_inc *= 0.1f;
    }

    setValue(*value+value_inc);
}

void UIFloatSlider::click(const vec2& pos) {
    float new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIFloatSlider::drag(const vec2& pos) {
    float new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIFloatSlider::setValue(float v) {
    *value = std::max(std::min(max,v), min);
}

void UIFloatSlider::drawContent() {

    float position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    drawSlider(position);
}

//UIIntSlider

UIIntSlider::UIIntSlider(int* value, int min, int max) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f) {
}

void UIIntSlider::scroll(bool up) {
    int value_inc = 1; //std::max(1, (max-min) / 100);

    if(!up) value_inc = -value_inc;

    setValue(*value+value_inc);
}

void UIIntSlider::click(const vec2& pos) {
    int new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIIntSlider::drag(const vec2& pos) {
    int new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIIntSlider::setValue(int v) {
    *value = std::max(std::min(max,v), min);
}

void UIIntSlider::drawContent() {

    float position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    drawSlider(position);
}

// UILabelFloatSlider

UILabelFloatSlider::UILabelFloatSlider(const std::string& label, float* value, float min, float max) : UILayout(true) {

    UIFloatSlider* slider = new UIFloatSlider(value, min, max);
    UIFloatLabel*  flabel = new UIFloatLabel(value, true);

    flabel->slider = slider;

    addElement(new UILabel(label, false, 120.0f));
    addElement(slider);
    addElement(flabel);

    padding = vec2(5.0f, 0.0f);
}

// UILabelIntSlider

UILabelIntSlider::UILabelIntSlider(const std::string& label, int* value, int min, int max) : UILayout(true) {

    UIIntSlider* slider = new UIIntSlider(value, min, max);
    UIIntLabel*  ilabel = new UIIntLabel(value, true);

    ilabel->slider = slider;

    addElement(new UILabel(label, false, 120.0f));
    addElement(slider);
    addElement(ilabel);

    padding = vec2(5.0f, 0.0f);
}
