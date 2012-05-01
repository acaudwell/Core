#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include "scroll_layout.h"
#include "group.h"

class UIConsole : public UIGroup {
    UIScrollLayout* history;
    UILabel* prompt;
    void updateHistory();
public:
    UIConsole();

    void toggle();
    void open();
    void close();

    void update(float dt);
};

class UIConsoleEntry : public UILabel {
protected:
    UIConsole* console;
public:
    UIConsoleEntry(UIConsole* console);
};

class UIConsolePrompt : public UILabel {
protected:
    UIConsole* console;
public:
    UIConsolePrompt(UIConsole* console);

    void updateRect();
    void updateContent();
    void drawContent();

    bool submit();
    void tab();
};

#endif
