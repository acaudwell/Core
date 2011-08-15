#ifndef UI_GROUP_h
#define UI_GROUP_h

#include "label.h"
#include "layout.h"

class UIGroup : public UIElement {

protected:
    UILayout* layout;

    float animation;
    float speed;

    vec2 old_group_rect;
    vec2 old_label_rect;

    bool open;
public:
    UILabel*  label;

    UIGroup(const std::string& groupname, bool open = true);
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
