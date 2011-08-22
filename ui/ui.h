#ifndef UI_H
#define UI_H

#include "../display.h"
#include "../util.h"
#include "../mousecursor.h"

#include <vector>
#include <list>

class UIElement;

class UI {
    UIElement* selectedElement;
    std::vector<UIElement*> elements;

    char toChar(SDL_KeyboardEvent *e);
public:
    FXFont font;

    UI();
    ~UI();

    void clear();

    void addElement(UIElement* e);

    UIElement* getSelected();

    bool elementsByType(std::list<UIElement*>& found, int type);

    UIElement* elementAt(const vec2& pos);
    UIElement* selectElementAt(const vec2& pos);

    void deselect();
    
    bool keyPress(SDL_KeyboardEvent *e);

    void processMouse(const MouseCursor& cursor, bool left_click, bool left_down);

    void update(float dt);

    void draw();
    void drawOutline();
};

#endif
