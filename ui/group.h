#ifndef UI_GROUP_h
#define UI_GROUP_h

#include "label.h"
#include "solidlayout.h"

class UIGroupBar : public UISolidLayout {
public:
    UILabel* label;
    UIGroupBar(const std::string& text);
    void setText(const std::string& text);
    void click(const vec2& pos);
};

class UIGroup : public UIElement {

protected:
    UILayout* layout;

    float animation;
    float speed;

    vec2 old_group_rect;
    vec2 old_label_rect;

    bool minimized;
    bool minimizable;
public:
    UIGroupBar* bar;

    UIGroup(const std::string& groupname, bool minimized = true, bool resizable = false);
    ~UIGroup();

    void setUI(UI* ui);

    bool elementsByType(std::list<UIElement*>& found, int type);
    UIElement* elementAt(const vec2& pos);

    int getType() { return UI_GROUP; };

    UILayout* getLayout() { return layout; };

    void setTitle(const std::string& text);

    void toggle();

    void update(float dt);
    void updatePos(const vec2& pos);

    void draw();
};

#endif
