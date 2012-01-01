#ifndef UI_SLIDER_H
#define UI_SLIDER_H

#include "element.h"
#include "label.h"
#include "layout.h"

class UISlider : public UIElement {
protected:
    TextureResource* slidertex;
    float slider_width;
    bool background;

    void drawSlider(float position);
public:
    UISlider(const std::string& slider_texure, float width);

    bool keyPress(SDL_KeyboardEvent *e, char c);

    virtual void scroll(bool up) {}; 

    int getType() { return UI_SLIDER; };
};

class UIFloatSlider : public UISlider {
public:
    float* value;
    float min;
    float max;

    UIFloatSlider(float* value, float min, float max);

    void scroll(bool up);
    void click(const vec2& pos);
    void drag(const vec2& pos);
    void setValue(float v);

    void drawContent();
};

class UIIntSlider : public UISlider {
public:
    int* value;
    int min;
    int max;

    UIIntSlider(int* value, int min, int max);

    void scroll(bool up);
    void click(const vec2& pos);
    void drag(const vec2& pos);
    void setValue(int v);

    void drawContent();
};

class UILabelFloatSlider : public UILayout {

public:
    UILabelFloatSlider(const std::string& label, float* value, float min, float max);
};

class UILabelIntSlider : public UILayout {

public:
    UILabelIntSlider(const std::string& label, int* value, int min, int max);
};

#endif
