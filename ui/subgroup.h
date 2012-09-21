#ifndef UI_SUBGROUP_h
#define UI_SUBGROUP_h

#include "label.h"
#include "image.h"
#include "solid_layout.h"

class UISubGroupBar : public UILayout {
    UIImage* expander;
public:
    UISubGroupBar(const std::string& text);

    void click(const vec2& pos);
};

class UISubGroup : public UILayout {
protected:
    UISubGroupBar* bar;
    UILayout* layout;
    UIAction* open_action;
public:
    UISubGroup(const std::string& groupname, bool minimized = true);

    void setOpenAction(UIAction* action) ;

    void setTitle(const std::string& text);

    UILayout* getLayout() const { return layout; };

    bool toggle();
    void minimize();
    void maximize();
};

#endif
