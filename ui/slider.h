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
    UIAction* action;

    void drawSlider(float position);
public:
    UISlider(const std::string& slider_texure, float width, UIAction* action = 0);

    bool keyPress(SDL_KeyboardEvent *e, char c);

    virtual void scroll(bool up) {};
    virtual void scale(bool up) {};

    void idle();

    int getType() const { return UI_SLIDER; };
};

class UIFloatSlider : public UISlider {
public:
    float* value;
    float min;
    float max;

    UIFloatSlider(float* value, float min, float max, UIAction* action = 0);

    void scale(bool up, float value_scale = 0.25f);
    void scroll(bool up);
    void click(const vec2& pos);
    void drag(const vec2& pos);

    void setFloat(float* f);

    void setValue(float v);

    void drawContent();
};

class UIIntSlider : public UISlider {
public:
    int* value;
    int min;
    int max;

    UIIntSlider(int* value, int min, int max, UIAction* action = 0);

    void scroll(bool up);
    void click(const vec2& pos);
    void drag(const vec2& pos);
    void setValue(int v);

    void drawContent();
};

class UILabelFloatSlider : public UILabelledElement {

    UIFloatSlider* slider;
    UIFloatLabel*  flabel;
public:
    UILabelFloatSlider(const std::string& label, float* value, float min, float max, UIAction* action = 0);

    void setFloat(float* f);

    void scale(bool up);
    void scale(bool up, float value_scale);
    void scroll(bool up);
};

class UILabelIntSlider : public UILabelledElement {

    UIIntSlider* slider;
public:
    UILabelIntSlider(const std::string& label, int* value, int min, int max, UIAction* action = 0);

    void scale(bool up);
    void scroll(bool up);
};

#endif
