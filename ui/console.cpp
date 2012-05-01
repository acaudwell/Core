#include "console.h"

// UIConsole

UIConsole::UIConsole()
    : UIGroup("Console", false, true) {

    history = new UIScrollLayout(vec2(420.0f, 100.0f));
    history->setDrawBackground(false);
    history->setFill(true);

    prompt  = new UIConsolePrompt(this);

    layout->addElement(history);
    layout->addElement(prompt);

    logger->setHistoryCapacity(100);
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
}

void UIConsole::updateHistory() {

    const std::deque<LoggerMessage>& history_log = logger->getHistory();

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
}

void UIConsole::update(float dt) {
    updateHistory();

    UIGroup::update(dt);
}

// UIConsoleEntry

UIConsoleEntry::UIConsoleEntry(UIConsole* console)
    : console(console), UILabel("", false, 420.0f) {
    setFillHorizontal(true);
}

// UIConsolePrompt

UIConsolePrompt::UIConsolePrompt(UIConsole* console)
    : console(console), UILabel("", true, 420.0f) {
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
