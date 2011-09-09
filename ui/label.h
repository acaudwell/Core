#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "element.h"

class UISlider;

class UILabel : public UIElement {
protected:
    float cursor_anim;
    
    void drawBackground();
public:
    vec4 bgcolour;
    vec3 font_colour;
    UISlider* slider;
    bool editable;
    std::string text;

    UILabel(const std::string& text, bool editable = false, float width = -1.0f);

    float width;

    bool keyPress(SDL_KeyboardEvent *e, char c);

    int getType() { return UI_LABEL; }

    void setWidth(float width);
    void setText(const std::string& text);

    void update(float dt);

    void updateRect();

    void drawContent();
};

class UIIntLabel : public UILabel {
    int* value;
public:
    UIIntLabel(int* value, bool editable);

    bool keyPress(SDL_KeyboardEvent *e, char c);

    void updateContent();
};

class UIFloatLabel : public UILabel {
    float* value;
public:
    UIFloatLabel(float* value, bool editable);

    bool keyPress(SDL_KeyboardEvent *e, char c);

    void updateContent();
};

#endif
