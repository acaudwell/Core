#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "element.h"
#include "layout.h"
#include "action.h"

class UISlider;
class UIFloatSlider;
class UIColourSlider;
class UILayout;

class UILabel : public UIElement {
protected:
    float cursor_anim;
    float expanded;

    bool text_changed;

    UIAction* action;

    std::string* value;

    void drawBackground();
public:
    vec4 bgcolour;
    vec4 selected_edit_bgcolour;
    vec4 edit_bgcolour;
    vec4 text_colour;

    UISlider* slider;
    std::string text;
    std::string display_text;

    UILabel(const std::string& text, bool editable = false, float width = -1.0f, UIAction* action = 0, std::string* value = 0);

    float width;

    bool keyPress(SDL_KeyboardEvent *e);

    int getType() const { return UI_LABEL; }

    virtual void tab();

    virtual void backspace();

    bool submit();

    void setWidth(float width);
    void setText(const std::string& text);
    void setTextColour(const vec4& colour);

    void update(float dt);

    void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    vec2 getRect();
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

    bool keyPress(SDL_KeyboardEvent *e);
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
    void setValue(float value);

    void scroll(bool up);
    void scale(bool up, float value_scale);
    
    bool keyPress(SDL_KeyboardEvent *e);
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

class UILabelString : public UILabelledElement {
public:
    UILabelString(const std::string& label, std::string* value, bool editable = false, UIAction* action = 0);
};

class UILabelVec3 : public UILabelledElement {
protected:
    vec3* value;
public:
    UILabelVec3(const std::string& label, vec3* value, bool editable = false, UIAction* action = 0);   
};

#endif
