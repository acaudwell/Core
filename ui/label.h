#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "element.h"
#include "action.h"

class UISlider;
class UIFloatSlider;
class UIColourSlider;

class UILabel : public UIElement {
protected:
    float cursor_anim;
    float expanded;

    bool text_changed;

    UIAction* action;

    void drawBackground();
public:
    vec4 bgcolour;
    vec4 selected_edit_bgcolour;
    vec4 edit_bgcolour;
    vec4 text_colour;

    UISlider* slider;
    std::string text;
    std::string display_text;

    UILabel(const std::string& text, bool editable = false, float width = -1.0f, UIAction* action = 0);

    float width;

    bool keyPress(SDL_KeyboardEvent *e, char c);

    int getType() { return UI_LABEL; }

    virtual void tab();
    virtual void backspace();

    void setWidth(float width);
    void setText(const std::string& text);
    void setTextColour(const vec4& colour);

    void update(float dt);

    UIElement* elementAt(const vec2& pos);

    void expandRect(const vec2& expand);
    void resetRect();

    void updateRect();

    void drawContent();

    void idle();
};

class UIIntLabel : public UILabel {
    int* value;
public:
    UIIntLabel(int* value, bool editable, UIAction* action = 0);

    bool keyPress(SDL_KeyboardEvent *e, char c);
    bool submit();

    void updateContent();
};

class UIFloatLabel : public UILabel {
protected:
    float* value;
public:
    UIFloatLabel(float* value, bool editable, UIAction* action = 0);
    UIFloatLabel(UIFloatSlider* slider, bool editable, UIAction* action = 0);

    void setValue(float* value);

    bool keyPress(SDL_KeyboardEvent *e, char c);
    bool submit();

    void setSelected(bool selected);

    void updateContent();
};

class UIColourLabel : public UIFloatLabel {
public:
    UIColourLabel(UIColourSlider* slider, bool editable);

    bool submit();

    void updateContent();
};

#endif
