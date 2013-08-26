#ifndef UI_SELECT_H
#define UI_SELECT_H

#include "action.h"
#include "label.h"
#include "solid_layout.h"

class UISelect;

class UIIntSelectAction : public UIAction {
    int* field;
    int value;
    UIAction* action;
public:
    UIIntSelectAction(int* field, int value, UIAction* action = 0);

    void perform();
};

class UIOptionLabel : public UILabel {
    UISelect* select;
    UIAction* action;
public:
    std::string value;

    UIOptionLabel(UISelect* select, const std::string& text, const std::string& value, UIAction* action = 0);
    UIOptionLabel(UISelect* select, const std::string& text, UIAction* action = 0);

    void click(const vec2& pos);

    bool submit();
};

class UISelect : public UISolidLayout {
    UILabel* label;
    UILayout* options_layout;

    UIOptionLabel* selected_option;

    TextureResource* selectex;
public:
    bool open;

    UISelect();
    ~UISelect();

    int getType() const { return UI_SELECT; };

    void setUI(UI* ui);

    void selectOption(UIOptionLabel* option, bool submit = true);

    UIOptionLabel* getSelectedOption();

    UIOptionLabel* addOption(const std::string& name, const std::string& value, bool select_option = false);
    UIOptionLabel* addOption(const std::string& name, UIAction* action, bool select_option = false);

    void click(const vec2& pos);

    void elementsAt(const vec2& pos, std::list<UIElement*>& elements_found);

    void updatePos(const vec2& pos);
    void update(float dt);

    void draw();
};

#endif
