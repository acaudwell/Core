#ifndef UI_FILE_SELECTOR_H
#define UI_FILE_SELECTOR_H

#include "group.h"
#include "select.h"
#include "scroll_layout.h"

class UIFileSelectorAction {
public:
    UIFileSelectorAction() {};
    virtual void perform(const  std::string& path) {};
};

class UIFileSelector : public UIGroup {

    UILabel* dir_path;
    UILabel* file_path;

    UIScrollLayout* listing;
    UISelect* filter_select;

    std::string previous_dir;

    std::string selected_path;
    std::string current_dir;

    std::string next_dir;
    UIOptionLabel* current_filter;

    UIFileSelectorAction* action;

public:
    UIFileSelector(const std::string& title, const std::string& dir, UIFileSelectorAction* action);

    void prettyDirectory(std::string& dir_string);

    void addFilter(const std::string& name, const std::string& extension, bool select = false);

    bool changeDir(const std::string& dir);
    void updateListing();

    void selectFile(const std::string& filename);
    void selectPath(const std::string& path);
    void confirm();

    const std::string& getCurrentDir() const;

    std::string autocomplete(const std::string& input, bool dirs_only = false);

    void toggle();
    void open();
    void close();

    void update(float dt);
};

class UIFileSelectorLabel : public UILabel {
protected:
    std::string path;
    bool directory;
    UIFileSelector* selector;
public:
    UIFileSelectorLabel(UIFileSelector* selector, const std::string& path);
    UIFileSelectorLabel(UIFileSelector* selector, const std::string& label, const std::string& path);

    bool isDir() const { return directory; };
    const std::string& getPath() const { return path; };

    void doubleClick(const vec2& pos);
    void click(const vec2& pos);

    bool submit();

    void updateContent();
};

class UIFileInputLabel : public UILabel {
    UIFileSelector* selector;
public:
    UIFileInputLabel(UIFileSelector* selector, const std::string& filename);

    void tab();

    bool submit();
};

class UIDirInputLabel : public UILabel {
    UIFileSelector* selector;
public:
    UIDirInputLabel(UIFileSelector* selector, const std::string& dirname);

    void tab();

    bool submit();
};

#endif
