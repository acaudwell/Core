#include "slider.h"

//UISlider

UISlider::UISlider(const std::string& slider_texture, float width, UIAction* action) : slider_width(width), action(action), UIElement() {
    slidertex  = texturemanager.grab(slider_texture);
    rect       = vec2(slider_width, 16.0f);
    background = true;
    selectable = true;
    scrollable = true;
    dragging   = false;
}

void UISlider::drawSlider(float position) {

    float slider_position = position * (slider_width-4.0f) + 4.0f;

    //background
    //drawQuad(vec2(slider_width,16.0f), vec4(0.0f, 0.0f, 1.0f, 0.5f));

    vec4 tint  = ui->getTintColour();
    vec4 alpha = ui->getAlpha();

    if(selected) tint = glm::clamp(tint + 0.75f, 0.0f, 1.0f);

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

bool UISlider::keyPress(SDL_KeyboardEvent *e) {

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
    dragging = false;
}


//UIFloatSlider

UIFloatSlider::UIFloatSlider(float* value, float min, float max, UIAction* action) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f, action) {
}

void UIFloatSlider::scroll(bool up) {
    scale(up, 0.1f);
}

void UIFloatSlider::scale(bool up, float value_scale) {

    if(!value) return;

    float value_inc = (min!=max) ? ((max-min) / 100.0f) : glm::max( 0.00001f, glm::abs(glm::max(1.0f, *value)) * 0.1f );

    if(!up) value_inc = -value_inc;

    setValue(*value + UIElement::granulaity(value_inc, value_scale));
}

void UIFloatSlider::click(const vec2& pos) {
    float percent = (pos.x - this->pos.x) / slider_width;

    float new_value;

    if(min!=max) {
        new_value = percent * (max-min) + min;
    } else {
        float v = *value;
        if(v == 0.0f) v = 1.0f;

        new_value = percent * 2.0 * v;
    }

    //debugLog("new value = %f", new_value);

    setValue(new_value);
}

void UIFloatSlider::drag(const vec2& pos) {
    if(!dragging) {
        old_value = *value;
        dragging = true;
    }

    float percent = (pos.x - this->pos.x) / slider_width;

    float new_value = (min!=max)
        ? percent * (max-min) + min
        : percent * 2.0 * old_value;

    //debugLog("drag: percent %f new_value %f", percent, new_value);

    setValue(new_value);
}

void UIFloatSlider::setFloat(float* f) {
    value = f;
}

void UIFloatSlider::setValue(float v) {
    if(!value) return;

    if(min!=max) {
        *value = std::max(std::min(max,v), min);
    } else {
        *value = v;
    }

    if(action != 0) action->perform();
}

void UIFloatSlider::drawContent() {
    if(!value) return;

    float position;

    if (min!=max) {
        position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    } else {
        if(dragging) {
            position = std::max(0.0f, std::min(1.0f, *value / (old_value * 2.0f)));
        } else {
            position = 0.5f;
        }
    }

    drawSlider(position);
}

//UIIntSlider

UIIntSlider::UIIntSlider(int* value, int min, int max, UIAction* action) :
    min(min), max(max),  value(value), UISlider("ui/slider.png", 128.0f, action) {
}

void UIIntSlider::scroll(bool up) {
    int value_inc = 1;

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
    if(min!=max) {
        *value = std::max(std::min(max,v), min);
    } else {
        *value = v;
    }

    if(action != 0) action->perform();
}

void UIIntSlider::drawContent() {

    float position = std::min(1.0f, ((*value) - min) / ((float)max-min));

    drawSlider(position);
}

// UILabelFloatSlider

UILabelFloatSlider::UILabelFloatSlider(const std::string& label, float* value, float min, float max, UIAction* action) : UILabelledElement(label) {

    slider = new UIFloatSlider(value, min, max, action);

    flabel = new UIFloatLabel(value, true, action);
    flabel->slider = slider;

    layout->addElement(slider);
    layout->addElement(flabel);

    padding = vec2(5.0f, 0.0f);
    scrollable = true;

    setFillHorizontal(true);
}

void UILabelFloatSlider::setFloat(float* f) {
    slider->setFloat(f);
    flabel->setValue(f);
  //  flabel->updateContent();
}

void UILabelFloatSlider::scroll(bool up) {
    slider->scroll(up);
    flabel->updateContent();
}

void UILabelFloatSlider::scale(bool up) {
    slider->scale(up);
    flabel->updateContent();
}

void UILabelFloatSlider::scale(bool up, float value_scale) {
    slider->scale(up, value_scale);
}

// UILabelIntSlider

UILabelIntSlider::UILabelIntSlider(const std::string& label, int* value, int min, int max, UIAction* action) : UILabelledElement(label) {

    slider = new UIIntSlider(value, min, max, action);

    ilabel = new UIIntLabel(value, true, action);
    ilabel->slider = slider;

    layout->addElement(slider);
    layout->addElement(ilabel);

    padding = vec2(5.0f, 0.0f);
    scrollable = true;

    setFillHorizontal(true);

}

void UILabelIntSlider::scroll(bool up) {
    slider->scroll(up);
    ilabel->updateContent();
}

void UILabelIntSlider::scale(bool up) {
    slider->scale(up);
    ilabel->updateContent();
}
