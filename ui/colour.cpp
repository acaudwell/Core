#include "colour.h"

UIColour::UIColour(vec3* colour) : colour(colour), UIElement() {
    active = false;
    toHSL();
}

void UIColour::toHSL() {
    toHSL(*colour, hue, saturation, lightness);
}

void UIColour::toHSL(const vec3& colour, float& hue, float& saturation, float& lightness) {

    float max = std::max(colour.x, std::max(colour.y, colour.z));
    float min = std::min(colour.x, std::min(colour.y, colour.z));

    lightness  = ( max + min ) * 0.5f;

    hue        = 0.0f;
    saturation = 0.0f;

    if(max != min) {

        float range = (max - min);

        if(lightness > 0.5f) {
            saturation = range / (2.0f - max - min);
        } else {
            saturation = range / (max + min);
        }

        if(max == colour.x) {

            hue = (colour.y - colour.z) / range;

            if(colour.y < colour.z) hue += 6.0f;
        }
        else if (max == colour.y) {
            hue = ((colour.z - colour.x) / range) + 2.0f;
        } else {
            hue = ((colour.x - colour.y) / range) + 4.0f;
        }

        hue /= 6.0f;
    }
}

void UIColour::toColour() {
    *colour = toColour(hue, saturation, lightness);
}

vec3 UIColour::toColour(float hue, float saturation, float lightness) {

    vec3 colour;

    if(hue < 0.16667f) {
        float h = hue / 0.16667f;
        colour = vec3(1.0f, 0.0f, 0.0f) * (1.0f - h) + vec3(1.0f, 1.0f, 0.0f) * h;
    } else if (hue < 0.33333f) {
        float h = (hue-0.16667f) / 0.16667f;
        colour = vec3(1.0f, 1.0f, 0.0f) * (1.0f - h) + vec3(0.0f, 1.0f, 0.0f) * h;
    } else if (hue < 0.5f) {
        float h = (hue-0.33333f) / 0.16667f;
        colour = vec3(0.0f, 1.0f, 0.0f) * (1.0f - h) + vec3(0.0f, 1.0f, 1.0f) * h;
    } else if (hue < 0.66667f) {
        float h = (hue-0.5f)/0.16667f;
        colour = vec3(0.0f, 1.0f, 1.0f) * (1.0f - h) + vec3(0.0f, 0.0f, 1.0f) * h;
    } else if (hue < 0.83333f) {
        float h = (hue-0.66667f)/0.16667f;
        colour = vec3(0.0f, 0.0f, 1.0f) * (1.0f - h) + vec3(1.0f, 0.0f, 1.0f) * h;
    } else {
        float h = (hue-0.83333f)/0.16667f;
        colour = vec3(1.0f, 0.0f, 1.0f) * (1.0f - h) + vec3(1.0f, 0.0f, 0.0f) * h;
    }

    if(lightness < 0.5f) {
        float l = lightness/0.5f;
        colour = vec3(0.0f, 0.0f, 0.0f) * (1.0f-l) + colour * l;
    } else {
        float l = (lightness-0.5)/0.5f;
        colour = vec3(1.0f, 1.0f, 1.0f) * l + colour * (1.0f-l);
    }

    colour = colour * saturation + vec3(lightness, lightness, lightness) * (1.0f - saturation);

    return colour;
}

void UIColour::updateRect() {
    rect.x = 14.0f;
    rect.y = 14.0f;
}

void UIColour::drawContent() {

    glDisable(GL_TEXTURE_2D);

    //background
    if(active) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    }

    drawQuad(vec2(0.0f, 0.0f), vec2(14.0f, 14.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));

    glColor4f(colour->x, colour->y, colour->z, 1.0f);
    drawQuad(vec2(1.0f, 1.0f), vec2(12.0f, 12.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f));

    glEnable(GL_TEXTURE_2D);
}

//UILabelColour

UILabelColour::UILabelColour(const std::string& label, vec3* value) : UILayout(true) {

    addElement(new UILabel(label, false, 120.0f));
    addElement(new UIColour(value));

    padding = vec2(5.0f);
}

UILabelColour::UILabelColour(const std::string& label, vec3* a, vec3* b, vec3* c) : UILayout(true) {

    addElement(new UILabel(label, false, 120.0f));
    addElement(new UIColour(a));
    addElement(new UIColour(b));
    addElement(new UIColour(c));

    padding = vec2(5.0f);
}

//UIColourSlider

UIColourSlider::UIColourSlider() : UISlider("ui/colour_slider.png", 128.0f) {
    background = false;
    attribute  = 0;
    colour     = 0;
}

void UIColourSlider::setColour(UIColour* colour) {
    this->colour    = colour;
    this->attribute = 0;
}

void UIColourSlider::drawGradient(const vec2& rect, const vec4& colour1, const vec4& colour2) {

    glBegin(GL_QUADS);

        glColor4fv(glm::value_ptr(colour1));
        glVertex2f(0.0f,    0.0f);

        glColor4fv(glm::value_ptr(colour2));
        glVertex2f(rect.x,  0.0f);

        glColor4fv(glm::value_ptr(colour2));
        glVertex2f(rect.x, rect.y);

        glColor4fv(glm::value_ptr(colour1));
        glVertex2f(0.0f,   rect.y);

    glEnd();
}

void UIColourSlider::updateRect() {
    rect.x = 128.0f;
    rect.y = 16.0f;
}

void UIColourSlider::drag(const vec2& pos) {
    if(!attribute) return;
    *attribute = std::max(0.0f, std::min(1.0f, ((pos.x - this->pos.x) / slider_width)));
    colour->toColour();
}

//UILightnessSlider

UILightnessSlider::UILightnessSlider() {
}

void UILightnessSlider::setColour(UIColour* colour) {
    this->colour    = colour;
    this->attribute = &(colour->lightness);
}

void UILightnessSlider::drawContent() {
    if(!colour) return;

    glDisable(GL_TEXTURE_2D);

    //lightness gradient
    vec4 colour1(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 colour2(colour->toColour(colour->hue, colour->saturation, 0.5f), 1.0f);
    vec4 colour3(1.0f, 1.0f, 1.0f, 1.0f);

    glPushMatrix();

    drawGradient(vec2(rect.x*0.5,rect.y), colour1, colour2);

    glTranslatef(rect.x*0.5f, 0.0f, 0.0f);
    drawGradient(vec2(rect.x*0.5,rect.y), colour2, colour3);

    glPopMatrix();

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    drawOutline(rect);

    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0f, 1.0f, 0.0f);
    drawSlider(*attribute);
}

//UISatSlider

UISatSlider::UISatSlider() {
}

void UISatSlider::setColour(UIColour* colour) {
    this->colour    = colour;
    this->attribute = &(colour->saturation);
}

void UISatSlider::drawContent() {
    if(!colour) return;

    float lightness = std::min(0.75f, std::max(0.25f, colour->lightness));

    glDisable(GL_TEXTURE_2D);

    //saturation gradient
    vec4 colour1 = vec4(colour->toColour(colour->hue, 0.0f, lightness), 1.0f);
    vec4 colour2 = vec4(colour->toColour(colour->hue, 1.0f, lightness), 1.0f);

    drawGradient(rect, colour1, colour2);

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    drawOutline(rect);

    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0f, 1.0f, 0.0f);
    drawSlider(*attribute);
}

//UIHueSlider

UIHueSlider::UIHueSlider() {
}

void UIHueSlider::setColour(UIColour* colour) {
    this->colour    = colour;
    this->attribute = &(colour->hue);
}

void UIHueSlider::drawContent() {
    if(!colour) return;

    float saturation = std::max(0.25f, colour->saturation);
    float lightness  = std::min(0.75f, std::max(0.25f, colour->lightness));

    glDisable(GL_TEXTURE_2D);

    glPushMatrix();

    vec4 colour1(colour->toColour(0.0f,     saturation, lightness), 1.0f);
    vec4 colour2(colour->toColour(0.16667f, saturation, lightness), 1.0f);
    vec4 colour3(colour->toColour(0.33333f, saturation, lightness), 1.0f);
    vec4 colour4(colour->toColour(0.5f,     saturation, lightness), 1.0f);
    vec4 colour5(colour->toColour(0.66667f, saturation, lightness), 1.0f);
    vec4 colour6(colour->toColour(0.83333f, saturation, lightness), 1.0f);
    vec4 colour7(colour->toColour(1.0f,     saturation, lightness), 1.0f);

    float width = rect.x*0.16667f;

    // draw hue
    drawGradient(vec2(width, rect.y), colour1, colour2);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2(width, rect.y), colour2, colour3);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2(width, rect.y), colour3, colour4);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2(width, rect.y), colour4, colour5);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2(width, rect.y), colour5, colour6);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2(width, rect.y), colour6, colour7);


    glPopMatrix();

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    drawOutline(rect);

    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0f, 1.0f, 0.0f);
    drawSlider(*attribute);
}
