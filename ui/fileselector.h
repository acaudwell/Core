#ifndef UI_FILE_SELECTOR_H
#define UI_FILE_SELECTOR_H

#include "group.h"
#include "scroll_layout.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

class UIFileSelector : public UIGroup {

    UILabel* dir_path;
    UILabel* file_path;
    UIScrollLayout* listing;
    std::string filter;
public:
    UIFileSelector(const std::string& title, const std::string& dir, const std::string& file);

    bool changeFilter(const std::string& filter);
    bool changeDir(const boost::filesystem::path& dir);    
    void updateListing();
};

class UIFileSelectorLabel : public UILabel {
protected:
    boost::filesystem::path path;
    bool directory;
    UIFileSelector* selector;
public:
    UIFileSelectorLabel(UIFileSelector* selector, const boost::filesystem::path& path);
    UIFileSelectorLabel(UIFileSelector* selector, const std::string& label, const boost::filesystem::path& path);

    void doubleClick();
    
    void updateContent();
};

class UIFileSelectLabel : public UILabel {
    UIFileSelector* selector;    
public:
    UIFileSelectLabel(UIFileSelector* selector, const std::string& filename);

    void submit();
};

class UIDirSelectLabel : public UILabel {
    UIFileSelector* selector;
public:
    UIDirSelectLabel(UIFileSelector* selector, const std::string& dirname);
    void submit();
};


#endif
