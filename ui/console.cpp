#include "console.h"

// UIConsole

UIConsole::UIConsole(const vec2& console_rect)
    : UIGroup("Console", false, true) {

    layout->setMinRect(console_rect);

    history = new UIScrollLayout(vec2(0.0f, 0.0f));
    history->setDrawBackground(false);
    history->setFill(true);

    message_count = 0;

    prompt  = new UIConsolePrompt(this);

    layout->addElement(history);
    layout->addElement(prompt);
}

UIConsole::~UIConsole() {

    for(auto it = commands.begin(); it != commands.end(); it++) {
        delete it->second;
    }

    commands.clear();
}

void UIConsole::showHide() {
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
    if(hidden) return;

    int message_count = Logger::getDefault()->getMessageCount();

    if(this->message_count == message_count) return;

    this->message_count = message_count;

    // TODO: test if history has changed since last call

    bool stick_to_end = history->vertical_scrollbar->atEnd();

    const std::deque<LoggerMessage>& history_log = Logger::getDefault()->getHistory();

    while(history->getElementCount() < history_log.size()) {
        history->addElement(new UIConsoleEntry(this));
    }

    int history_size = history->getElementCount();

    int i=0;
    for(auto it = history_log.rbegin(); it != history_log.rend(); it++) {
        const LoggerMessage& l = (*it);

        UIConsoleEntry* entry = (UIConsoleEntry*) history->getElement(history_size-i-1);

        switch(l.level) {
            case LOG_LEVEL_INFO:
                entry->setTextColour(vec4(1.0f));
                break;
            case LOG_LEVEL_DEBUG:
                entry->setTextColour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
                break;
            case LOG_LEVEL_CONSOLE:
                entry->setTextColour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
                break;
            case LOG_LEVEL_SCRIPT:
                entry->setTextColour(vec4(1.0f, 0.5f, 0.25f, 1.0f));
                break;
            case LOG_LEVEL_ERROR:
                entry->setTextColour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
                break;
            case LOG_LEVEL_WARN:
                entry->setTextColour(vec4(1.0f, 0.5f, 0.2f, 1.0f));
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

void UIConsole::registerCommand(UIConsoleCommand* command) {
    assert(getCommand(command->getName()) == 0);

    commands[command->getName()] = command;
}

UIConsoleCommand* UIConsole::getCommand(const std::string& name) {

    auto it = commands.find(name);

    if(it != commands.end()) {
        return it->second;
    }

    return 0;
}

bool UIConsole::executeCommand(const std::string& command_string) {

    consoleLog(command_string.c_str());

    size_t s = command_string.find(" ");

    std::string command_name;
    std::string command_args;

    if(s != std::string::npos) {
        command_name = command_string.substr(0, s);
        if(s != command_string.size()-1) {
            command_args = command_string.substr(s+1);
        }
   } else {
        command_name = command_string;
    }

    UIConsoleCommand* command = getCommand(command_name);

    if(!command) {
        errorLog("no such console command '%s'", command_name.c_str());
        return false;
    }

    return command->execute(command_args);
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


//UIConsoleCommand

UIConsoleCommand::UIConsoleCommand(const std::string& name)
    : name(name) {
}

const std::string& UIConsoleCommand::getName() const {
    return name;
}

bool UIConsoleCommand::execute(const std::string& args) {

    if(!args.empty()) return false;

    return execute();
}

// UIConsolePrompt

UIConsolePrompt::UIConsolePrompt(UIConsole* console)
    : console(console), UILabel("", true), history_index(-1) {
    setFillHorizontal(true);
}

bool UIConsolePrompt::submit() {

    if(text.empty()) return false;

    std::string command = text;

    setText("");

    command_history.push_back(command);
    history_index = -1;

    return console->executeCommand(command);
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

bool UIConsolePrompt::keyPress(SDL_KeyboardEvent *e) {

    if(e->keysym.sym == SDLK_BACKQUOTE) {
        console->showHide();
        return true;
    }

    if(e->keysym.sym == SDLK_UP) {
        if(history_index == -1 && !command_history.empty()) {
            history_index = command_history.size() - 1;
            setText(command_history[history_index]);
        } else if(history_index > 0) {
            history_index--;
            setText(command_history[history_index]);
        }
        return true;
    }

    if(e->keysym.sym == SDLK_DOWN) {
        if(history_index < command_history.size()-1) {
            history_index++;
            setText(command_history[history_index]);
        } else {
            history_index = -1;
            setText("");
        }

        return true;
    }

    return UILabel::keyPress(e);
}

