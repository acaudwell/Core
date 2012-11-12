#ifndef UI_ACTION_H
#define UI_ACTION_H

class UIElement;

class UIAction {
public:
    UIAction() {};
    virtual ~UIAction() {};

    virtual void perform() {};
    virtual void idle() {};
};

class UIElementAction : public UIAction {
protected:
    UIElement* element;
public:
    UIElementAction(UIElement* element) : element(element) {};
};

#endif
