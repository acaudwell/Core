#ifndef UI_H
#define UI_H

#include "../display.h"
#include "../mousecursor.h"

#include <set>
#include <list>

class UIElement;
class UIColour;

class UI {
    Shader* shader;

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
    vec4 tint_colour;
    vec4 ui_alpha;

    bool left_pressed;
    bool left_drag;
    bool scrolling;

    vec2 cursor_pos;

    virtual UIElement* click(const MouseCursor& cursor);
    virtual UIElement* drag(const MouseCursor& cursor);
    virtual UIElement* scroll(MouseCursor& cursor);
public:
    FXFont font;

    UI();
    ~UI();

    void clear();

    void drawText(float x, float y, const char *str, ...);
    void drawText(float x, float y, const std::string& text);

    void addElement(UIElement* e);
    void removeElement(UIElement* e);

    UIElement* getSelected();

    bool acceptingTextInput();
    
    bool elementsByType(std::list<UIElement*>& found, int type);

    void elementsAt(const vec2& pos, std::list<UIElement*>& found_elements);

    UIElement* scrollableElementAt(const vec2& pos);
    UIElement* selectElementAt(const vec2& pos);

    void selectElement(UIElement* element);

    void deselect();

    bool keyPress(SDL_KeyboardEvent *e);

    virtual UIElement* processMouse(MouseCursor& cursor);

    virtual vec4 getSolidColour();
    virtual vec4 getBackgroundColour();
    virtual vec4 getTextColour();
    virtual vec4 getTintColour();
    virtual vec4 getAlpha();

    void textEdit(SDL_TextEditingEvent* e);
    void textInput(SDL_TextInputEvent* e);
    
    UIColour* getActiveColour();

    void update(float dt);

    void setTextured(bool textured);
    void setIntensity(float intensity);

    void draw();
    void drawOutline();
};

#endif
