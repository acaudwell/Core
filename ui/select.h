#ifndef UI_SELECT_H
#define UI_SELECT_H

#include "label.h"
#include "layout.h"

class UISelect : public UILabel {
    std::vector< std::pair<std::string,std::string> > options;
    UILayout* options_layout;
    int selected_option;
public:
    UISelect();
    ~UISelect();
    
    UIElement* elementAt(const vec2& pos);

    int getType() { return UI_SELECT; };

    void setUI(UI* ui);

    void selectOption(int index);
    void addOption(const std::string& name, const std::string& value);
};

#endif
