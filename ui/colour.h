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
    
    int getType() { return UI_COLOUR; };

    void updateRect();
    void drawContent();
};

class UILabelColour : public UILayout {

public:
    UILabelColour(const std::string& label, vec3* value);
};

class UIColourSlider : public UISlider {
protected:
    void drawGradient(const vec2& rect, const vec4& colour1, const vec4& colour2);
public:
    UIColourSlider();

    UIColour* colour;
    float* attribute;

    void selectValueAt(const vec2& pos);

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
