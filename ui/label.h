#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "element.h"

class UISlider;

class UILabel : public UIElement {
protected:
    std::vector<TextureResource*> labeltex;

    bool opaque;
    bool fixed;

    void drawBackground();

    float cursor_anim;
public:
    UISlider* slider;
    bool editable;
    std::string text;

    UILabel(const std::string& text, bool editable = false, bool opaque = false, float width = -1.0f);
    ~UILabel();

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
    void updateRect();
};

class UIFloatLabel : public UILabel {
    float* value;
public:
    UIFloatLabel(float* value, bool editable);

    bool keyPress(SDL_KeyboardEvent *e, char c);

    void updateContent();
    void updateRect();
};

#endif
