#ifndef UI_H
#define UI_H

#include "../display.h"
#include "../util.h"

#include <vector>
#include <list>

class UIElement;

class UI {
    UIElement* selectedElement;
    std::vector<UIElement*> elements;
public:
    FXFont font;

    UI();
    ~UI();

    void clear();

    void addElement(UIElement* e);

    UIElement* getSelected();

    bool elementsByType(std::list<UIElement*>& found, int type);

    UIElement* elementAt(const vec2f& pos);
    UIElement* selectElementAt(const vec2f& pos);

    void update(float dt);

    void draw();
    void drawOutline();
};

#endif
