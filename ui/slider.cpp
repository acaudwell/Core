#include "slider.h"

//UISlider

UISlider::UISlider(const std::string& slider_texture, float width) : slider_width(width), UIElement() {
    slidertex  = texturemanager.grab(slider_texture);
    rect       = vec2(slider_width, 16.0f);
    background = true;
    editable   = true;
}

void UISlider::drawSlider(float position) {

    float slider_position = position * (slider_width-4.0f) + 4.0f;

    //background
    //drawQuad(vec2(slider_width,16.0f), vec4(0.0f, 0.0f, 1.0f, 0.5f));

    vec4 alpha = ui->getAlpha();

    if(background) {

        glDisable(GL_TEXTURE_2D);

        glLineWidth(1.0f);

        glColor4f(0.0f, 0.0f, 0.0f, alpha.w);
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 3.0f);
            glVertex2f(slider_width, 3.0f);
            glVertex2f(slider_width, 11.0f);
            glVertex2f(0.0f, 11.0f);
        glEnd();


        glPushMatrix();
            glTranslatef(0.5f, 0.5f, 0.0f);

            if(selected) glColor4f(0.75f, 1.0f, 0.75f, alpha.w);
            else glColor4f(0.0f, 1.0f, 0.0f, alpha.w);

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

    glColor4fv(glm::value_ptr(ui->getSolidColour()));

    //slider
    drawQuad(vec2(slider_position-4.0, 0.0f), vec2(16.0,16.0), vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

bool UISlider::keyPress(SDL_KeyboardEvent *e, char c) {

    switch(e->keysym.sym) {
        case SDLK_MINUS:
                scroll(false);
                return true;
            break;
        case SDLK_EQUALS:
                scroll(true);
                return true;
            break;
    }

    return false;
}

//UIFloatSlider

UIFloatSlider::UIFloatSlider(float* value, float min, float max) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f) {
}

void UIFloatSlider::scroll(bool up) {

    float value_inc = (max-min) / 100.0f;

    if(!up) value_inc = -value_inc;

    bool left_ctrl  = false;
    bool left_shift = false;

#if SDL_VERSION_ATLEAST(1,3,0)
    Uint8* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_LCTRL])  left_ctrl  = true;
    if(keystate[SDL_SCANCODE_LSHIFT]) left_shift = true;
#else
    Uint8* keystate = SDL_GetKeyState(NULL);

    if(keystate[SDLK_LCTRL])  left_ctrl  = true;
    if(keystate[SDLK_LSHIFT]) left_shift = true;
#endif

    if(left_ctrl) {
        value_inc *= 0.1f;
    }

    if(left_shift) {
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
