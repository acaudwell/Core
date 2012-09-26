#ifndef UI_COLOUR_H
#define UI_COLOUR_H

#include "element.h"
#include "slider.h"
#include "label.h"
#include "group.h"

class UIColour : public UIElement {
public:
    bool active;

    vec3* colour;

    float hue;
    float saturation;
    float lightness;

    UIColour(vec3* colour);

    void toHSL();
    void toColour();

    static void  toHSL(const vec3& colour, float& hue, float& saturation, float& lightness);
    static vec3 toColour(float hue, float saturation, float lightness);

    void setColour(vec3* colour);

    int getType() const { return UI_COLOUR; };

    void updateRect();
    void drawContent();
};

class UILabelColour : public UILabelledElement {

public:
    UIColour* ui_colour;

    UILabelColour(const std::string& label);
    UILabelColour(const std::string& label, vec3* value);
    UILabelColour(const std::string& label, vec3* a, vec3* b, vec3* c);
    UILabelColour(const std::string& label, vec3* a, vec3* b, vec3* c, vec3* d, vec3* e, vec3* f);

    void setColour(vec3* value);

};

class UIColourSlider : public UISlider {
protected:
    void drawGradient(const vec2& rect, const vec4& colour1, const vec4& colour2);
public:
    UIColourSlider();

    UIColour* colour;
    float* attribute;

    void drag(const vec2& pos);
    void setValue(float value);
    void scroll(bool up);

    virtual void setColour(UIColour* colour);

    void updateRect();
};

class UIHueSlider : public UIColourSlider {
public:
    UIHueSlider();

    void drawContent();

    void setColour(UIColour* colour);
};

class UILightnessSlider : public UIColourSlider {
public:
    UILightnessSlider();

    void drawContent();

    void setColour(UIColour* colour);

};

class UISatSlider : public UIColourSlider {
public:
    UISatSlider();

    void drawContent();

    void setColour(UIColour* colour);
};

#endif
