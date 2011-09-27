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

    float double_click_interval;
    float double_click_timer;
    
    bool interaction;
    
    char toChar(SDL_KeyboardEvent *e);
protected:
    vec4 background_colour;
    vec4 solid_colour;
    vec4 text_colour;
    vec4 ui_alpha;
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

    virtual void click(const MouseCursor& cursor);
    virtual void drag(const MouseCursor& cursor);

    virtual vec4 getSolidColour();
    virtual vec4 getBackgroundColour();
    virtual vec4 getTextColour();
    virtual vec4 getAlpha();
    
    void update(float dt);

    void draw();
    void drawOutline();
};

#endif
