#include "console.h"

// UIConsole

UIConsole::UIConsole(const vec2& console_rect)
    : UIGroup("Console", false, true) {

    history = new UIScrollLayout(vec2(console_rect.x, console_rect.y));
    history->setDrawBackground(false);
    history->setFill(true);

    prompt  = new UIConsolePrompt(this, console_rect.x);

    layout->addElement(history);
    layout->addElement(prompt);
}

void UIConsole::toggle() {
    if(hidden) open();
    else close();
}

void UIConsole::open() {
    hidden=false;
    if(ui!=0) ui->selectElement(prompt);
}

void UIConsole::close() {
    hidden=true;
    if(ui!=0) ui->deselect();
}

void UIConsole::updateHistory() {

    // TODO: test if history has changed since last call

    bool stick_to_end = history->vertical_scrollbar->atEnd();

    const std::deque<LoggerMessage>& history_log = Logger::getDefault()->getHistory();

    while(history->getElementCount() < history_log.size()) {
        history->addElement(new UIConsoleEntry(this));
    }

    int history_size = history->getElementCount();

    int i=0;
    for(std::deque<LoggerMessage>::const_reverse_iterator it = history_log.rbegin(); it != history_log.rend(); it++) {
        const LoggerMessage& l = (*it);

        UIConsoleEntry* entry = (UIConsoleEntry*) history->getElement(history_size-i-1);

        switch(l.level) {
            case LOG_LEVEL_INFO:
                entry->setTextColour(vec4(1.0f));
                break;
            case LOG_LEVEL_DEBUG:
                entry->setTextColour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
                break;
            case LOG_LEVEL_WARN:
                entry->setTextColour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
                break;
            default:
                entry->setTextColour(vec4(1.0f));
                break;
        }

        entry->setText(l.message);
        i++;
    }

    if(stick_to_end) {
        history->vertical_scrollbar->stickToEnd();
    }
}

void UIConsole::update(float dt) {
    updateHistory();

    UIGroup::update(dt);
}

// UIConsoleEntry

UIConsoleEntry::UIConsoleEntry(UIConsole* console)
    : console(console), UILabel("", false) {
    setFillHorizontal(true);
}

// UIConsolePrompt

UIConsolePrompt::UIConsolePrompt(UIConsole* console, int width)
    : console(console), UILabel("", true, width) {
    setFillHorizontal(true);
}

bool UIConsolePrompt::submit() {

    if(!text.empty()) {
        infoLog("%s", text.c_str());
        setText("");
        return true;
    }

    return false;
}

void UIConsolePrompt::tab() {
    //auto complete?
}

void UIConsolePrompt::updateRect() {
    UILabel::updateRect();
}

void UIConsolePrompt::updateContent() {
    bgcolour = selected ? vec4(1.0f, 1.0f, 1.0f, 0.15f) : vec4(0.0f);
}

void UIConsolePrompt::drawContent() {
    UILabel::drawContent();
}

bool UIConsolePrompt::keyPress(SDL_KeyboardEvent *e, char c) {

    if(c == SDLK_BACKQUOTE) {
        console->toggle();
        return true;
    }

    return UILabel::keyPress(e, c);
}

