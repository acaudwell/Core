#ifndef UI_GROUP_h
#define UI_GROUP_h

#include "label.h"
#include "solid_layout.h"

class UIGroupBar : public UISolidLayout {
public:
    UILabel* label;
    UIGroupBar(const std::string& text);
    void setText(const std::string& text);
    void click(const vec2& pos);
};

class UIGroup : public UILayout {

protected:
    UILayout* layout;
    UIAction* open_action;
    UIGroupBar* bar;

    bool minimizable;
public:
    UIGroup(const std::string& groupname, bool minimized = false, bool resizable = false);
    virtual ~UIGroup() {};

    void setOpenAction(UIAction* action);

    int getType() const { return UI_GROUP; };

    UILayout* getLayout() { return layout; };

    void setTitle(const std::string& text);

    virtual void toggle();
    virtual void minimize();
    virtual void maximize();
};

#endif
