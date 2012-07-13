#include "slider.h"

//UISlider

UISlider::UISlider(const std::string& slider_texture, float width, UIAction* action) : slider_width(width), action(action), UIElement() {
    slidertex  = texturemanager.grab(slider_texture);
    rect       = vec2(slider_width, 16.0f);
    background = true;
    editable   = true;
    scrollable = true;
}

void UISlider::drawSlider(float position) {

    float slider_position = position * (slider_width-4.0f) + 4.0f;

    //background
    //drawQuad(vec2(slider_width,16.0f), vec4(0.0f, 0.0f, 1.0f, 0.5f));

    vec4 tint  = ui->getTintColour();
    vec4 alpha = ui->getAlpha();

    if(selected) tint = glm::clamp(tint + 0.75, 0.0, 1.0);

    if(background) {

        ui->setTextured(false);

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

            glColor4fv(glm::value_ptr(tint));

            glBegin(GL_LINE_STRIP);
                glVertex2f(0.0f, 3.0f);
                glVertex2f(slider_width, 3.0f);
                glVertex2f(slider_width, 11.0f);
                glVertex2f(0.0f, 11.0f);
                glVertex2f(0.0f, 3.0f);
            glEnd();

        glPopMatrix();

        ui->setTextured(true);
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
        default:
            break;
    }

    return false;
}

void UISlider::idle() {
    if(action != 0) action->idle();
}


//UIFloatSlider

UIFloatSlider::UIFloatSlider(float* value, float min, float max, UIAction* action) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f, action) {
}

void UIFloatSlider::scroll(bool up) {

    if(!value) return;

    float value_inc = (max-min) / 100.0f;

    if(!up) value_inc = -value_inc;

    setValue(*value + UIElement::granulaity(value_inc, 0.1));
}

void UIFloatSlider::scale(bool up, float value_scale) {

    if(!value) return;

    bool left_ctrl, left_shift;
    getModifiers(left_ctrl, left_shift);

    if(left_ctrl) {
        value_scale *= 0.1f;
    }

    if(left_shift) {
        value_scale *= 0.1f;
    }

    if(!up) value_scale = -value_scale;

    setValue(*value*(1.0+value_scale));
}

void UIFloatSlider::click(const vec2& pos) {
    float new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIFloatSlider::drag(const vec2& pos) {
    float new_value = ((pos.x - this->pos.x) / slider_width) * (max-min) + min;

    setValue(new_value);
}

void UIFloatSlider::setFloat(float* f) {
    value = f;
}

void UIFloatSlider::setValue(float v) {
    if(!value) return;

    *value = std::max(std::min(max,v), min);
    if(action != 0) action->perform();
}

void UIFloatSlider::drawContent() {
    if(!value) return;
    float position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    drawSlider(position);
}

//UIIntSlider

UIIntSlider::UIIntSlider(int* value, int min, int max, UIAction* action) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f, action) {
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
    if(action != 0) action->perform();
}

void UIIntSlider::drawContent() {

    float position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    drawSlider(position);
}

// UILabelFloatSlider

UILabelFloatSlider::UILabelFloatSlider(const std::string& label, float* value, float min, float max, UIAction* action) : UILayout(true) {

    slider = new UIFloatSlider(value, min, max, action);

    flabel = new UIFloatLabel(value, true, action);
    flabel->slider = slider;

    addElement(new UILabel(label, false, 120.0f));
    addElement(slider);
    addElement(flabel);

    padding = vec2(5.0f, 0.0f);
    scrollable = true;
}

void UILabelFloatSlider::setFloat(float* f) {
    slider->setFloat(f);
    flabel->setValue(f);
  //  flabel->updateContent();
}

void UILabelFloatSlider::scroll(bool up) {
    slider->scroll(up);
}

void UILabelFloatSlider::scale(bool up) {
    slider->scale(up);
}

void UILabelFloatSlider::scale(bool up, float value_scale) {
    slider->scale(up, value_scale);
}

// UILabelIntSlider

UILabelIntSlider::UILabelIntSlider(const std::string& label, int* value, int min, int max, UIAction* action) : UILayout(true) {

    slider = new UIIntSlider(value, min, max, action);

    UIIntLabel*  ilabel = new UIIntLabel(value, true, action);

    ilabel->slider = slider;

    addElement(new UILabel(label, false, 120.0f));
    addElement(slider);
    addElement(ilabel);

    padding = vec2(5.0f, 0.0f);
    scrollable = true;

}

void UILabelIntSlider::scroll(bool up) {
    slider->scroll(up);
}

void UILabelIntSlider::scale(bool up) {
    slider->scale(up);
}
