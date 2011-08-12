#include "colour.h"

UIColour::UIColour(vec3f* colour) : colour(colour), UIElement() {
    active = false;
    toHSL();
}

void UIColour::toHSL() {
    toHSL(*colour, hue, saturation, lightness);
}

void UIColour::toHSL(const vec3f& colour, float& hue, float& saturation, float& lightness) {

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

vec3f UIColour::toColour(float hue, float saturation, float lightness) {
    
    vec3f colour;
    
    if(hue < 0.16667f) {
        float h = hue / 0.16667f;
        colour = vec3f(1.0f, 0.0f, 0.0f) * (1.0f - h) + vec3f(1.0f, 1.0f, 0.0f) * h;
    } else if (hue < 0.33333f) {
        float h = (hue-0.16667f) / 0.16667f;
        colour = vec3f(1.0f, 1.0f, 0.0f) * (1.0f - h) + vec3f(0.0f, 1.0f, 0.0f) * h;
    } else if (hue < 0.5f) {
        float h = (hue-0.33333f) / 0.16667f;
        colour = vec3f(0.0f, 1.0f, 0.0f) * (1.0f - h) + vec3f(0.0f, 1.0f, 1.0f) * h;   
    } else if (hue < 0.66667f) {        
        float h = (hue-0.5f)/0.16667f;
        colour = vec3f(0.0f, 1.0f, 1.0f) * (1.0f - h) + vec3f(0.0f, 0.0f, 1.0f) * h;
    } else if (hue < 0.83333f) {
        float h = (hue-0.66667f)/0.16667f;
        colour = vec3f(0.0f, 0.0f, 1.0f) * (1.0f - h) + vec3f(1.0f, 0.0f, 1.0f) * h;
    } else {
        float h = (hue-0.83333f)/0.16667f;
        colour = vec3f(1.0f, 0.0f, 1.0f) * (1.0f - h) + vec3f(1.0f, 0.0f, 0.0f) * h;
    }
  
    if(lightness < 0.5f) {
        float l = lightness/0.5f;
        colour = vec3f(0.0f, 0.0f, 0.0f) * (1.0-l) + colour * l;
    } else {
        float l = (lightness-0.5)/0.5f;
        colour = vec3f(1.0f, 1.0f, 1.0f) * l + colour * (1.0-l);
    }

    colour = colour * saturation + vec3f(lightness, lightness, lightness) * (1.0 - saturation);
        
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

    drawQuad(vec2f(0.0f, 0.0f), vec2f(14.0f, 14.0f), vec4f(0.0f, 0.0f, 1.0f, 1.0f));

    glColor4f(colour->x, colour->y, colour->z, 1.0f);
    drawQuad(vec2f(1.0f, 1.0f), vec2f(12.0f, 12.0f), vec4f(0.0f, 0.0f, 1.0f, 1.0f));

    glEnable(GL_TEXTURE_2D);
}

//UILabelColour

UILabelColour::UILabelColour(const std::string& label, vec3f* value) : UILayout(true) {

    addElement(new UILabel(label));
    addElement(new UIColour(value));

    padding = 5.0f;
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

void UIColourSlider::drawGradient(const vec2f& rect, const vec4f& colour1, const vec4f& colour2) {

    glBegin(GL_QUADS);

        glColor4fv(colour1);
        glVertex2f(0.0f,    0.0f);

        glColor4fv(colour2);
        glVertex2f(rect.x,  0.0f);

        glColor4fv(colour2);
        glVertex2f(rect.x, rect.y);

        glColor4fv(colour1);
        glVertex2f(0.0f,   rect.y);

    glEnd();
}

void UIColourSlider::updateRect() {
    rect.x = 128.0f;
    rect.y = 16.0f;
}

void UIColourSlider::selectValueAt(const vec2f& pos) {
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
    vec4f colour1(0.0f, 0.0f, 0.0f, 1.0f);
    vec4f colour2(colour->toColour(colour->hue, colour->saturation, 0.5f), 1.0f);
    vec4f colour3(1.0f, 1.0f, 1.0f, 1.0f);

    glPushMatrix();

    drawGradient(vec2f(rect.x*0.5,rect.y), colour1, colour2);

    glTranslatef(rect.x*0.5f, 0.0f, 0.0f);
    drawGradient(vec2f(rect.x*0.5,rect.y), colour2, colour3);

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
    vec4f colour1 = vec4f(colour->toColour(colour->hue, 0.0f, lightness), 1.0f);    
    vec4f colour2 = vec4f(colour->toColour(colour->hue, 1.0f, lightness), 1.0f);
    
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
    
    vec4f colour1(colour->toColour(0.0f,     saturation, lightness), 1.0f);
    vec4f colour2(colour->toColour(0.16667f, saturation, lightness), 1.0f);
    vec4f colour3(colour->toColour(0.33333f, saturation, lightness), 1.0f);
    vec4f colour4(colour->toColour(0.5f,     saturation, lightness), 1.0f);
    vec4f colour5(colour->toColour(0.66667f, saturation, lightness), 1.0f);
    vec4f colour6(colour->toColour(0.83333f, saturation, lightness), 1.0f);
    vec4f colour7(colour->toColour(1.0f,     saturation, lightness), 1.0f);
    
    float width = rect.x*0.16667f;
    
    // draw hue
    drawGradient(vec2f(width, rect.y), colour1, colour2);

    glTranslatef(width, 0.0f, 0.0f);

    drawGradient(vec2f(width, rect.y), colour2, colour3);

    glTranslatef(width, 0.0f, 0.0f);
    
    drawGradient(vec2f(width, rect.y), colour3, colour4);

    glTranslatef(width, 0.0f, 0.0f);
    
    drawGradient(vec2f(width, rect.y), colour4, colour5);

    glTranslatef(width, 0.0f, 0.0f);
    
    drawGradient(vec2f(width, rect.y), colour5, colour6);

    glTranslatef(width, 0.0f, 0.0f);
    
    drawGradient(vec2f(width, rect.y), colour6, colour7);

    
    glPopMatrix();

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    drawOutline(rect);

    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0f, 1.0f, 0.0f);
    drawSlider(*attribute);
}
