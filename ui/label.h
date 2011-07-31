#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "element.h"

class UILabel : public UIElement {
protected:
    std::vector<TextureResource*> labeltex;

    bool editable;
    bool opaque;

    void drawBackground();
public:
    std::string text;

    UILabel(const std::string& text, bool editable = false, bool opaque = false);
    ~UILabel();

    float width;

    int getType() { return UI_LABEL; }

    void setText(const std::string& text);

    void updateRect();

    void drawContent();
};

class UIIntLabel : public UILabel {
    int* value;
public:
    UIIntLabel(int* value, bool editable);

    void updateRect();
};

class UIFloatLabel : public UILabel {
    float* value;
public:
    UIFloatLabel(float* value, bool editable);

    void updateRect();
};

#endif
