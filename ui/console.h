#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include "scroll_layout.h"
#include "group.h"

class UIConsoleCommand {
protected:
    std::string name;
public:
    UIConsoleCommand(const std::string& name);
    virtual ~UIConsoleCommand() {};
    
    const std::string& getName() const;

    virtual bool execute(const std::string& args);
    virtual bool execute() { return false; };
};

class UIConsole : public UIGroup {
    UIScrollLayout* history;
    UILabel* prompt;

    int message_count;

    void updateHistory();

    std::map<std::string, UIConsoleCommand*> commands;
public:
    UIConsole(const vec2& console_rect);
    ~UIConsole();

    void registerCommand(UIConsoleCommand* command);

    UIConsoleCommand* getCommand(const std::string& name);
    bool executeCommand(const std::string& command_string);

    void showHide();
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
    std::vector<std::string> command_history;
    int history_index;
public:
    UIConsolePrompt(UIConsole* console);

    void updateRect();
    void updateContent();
    void drawContent();

    bool keyPress(SDL_KeyboardEvent *e);

    bool submit();
    void tab();
};

#endif
