#ifndef UI_FILE_SELECTOR_H
#define UI_FILE_SELECTOR_H

#include "group.h"
#include "select.h"
#include "scroll_layout.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

class UIFileSelectorAction {
public:
    UIFileSelectorAction() {};
    virtual void perform(const  boost::filesystem::path& path) {};
};

class UIFileSelector : public UIGroup {

    UILabel* dir_path;
    UILabel* file_path;

    UIScrollLayout* listing;
    UISelect* filter_select;

    std::string previous_dir;

    boost::filesystem::path selected_path;
    boost::filesystem::path current_dir;

    std::string next_dir;
    UIOptionLabel* current_filter;

    UIFileSelectorAction* action;

    bool parentPath(const boost::filesystem::path& path, boost::filesystem::path& parent_path);
public:
    UIFileSelector(const std::string& title, const std::string& dir, UIFileSelectorAction* action);

    void prettyDirectory(std::string& dir_string);

    void addFilter(const std::string& name, const std::string& extension, bool select = false);

    bool changeDir(const boost::filesystem::path& dir);
    void updateListing();

    void selectFile(const boost::filesystem::path& filename);
    void selectPath(const boost::filesystem::path& path);
    void confirm();

    const boost::filesystem::path& getCurrentDir() const;

    std::string autocomplete(const std::string& input, bool dirs_only = false);

    void toggle();
    void open();
    void close();

    void update(float dt);
};

class UIFileSelectorLabel : public UILabel {
protected:
    boost::filesystem::path path;
    bool directory;
    UIFileSelector* selector;
public:
    UIFileSelectorLabel(UIFileSelector* selector, const boost::filesystem::path& path);
    UIFileSelectorLabel(UIFileSelector* selector, const std::string& label, const boost::filesystem::path& path);

    bool isDir() const { return directory; };
    const boost::filesystem::path& getPath() const { return path; };

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
