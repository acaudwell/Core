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
public:
    UIFileSelector(const std::string& title, const std::string& dir, const std::string& file);

    void changeDir(const boost::filesystem::path& dir);    
    void updateListing();
};

class UIFileSelectorLabel : public UILabel {
protected:
    boost::filesystem::path path;
    bool directory;
    UIFileSelector* selector;
public:
    UIFileSelectorLabel(UIFileSelector* selector, const boost::filesystem::path& path);

    void doubleClick();
    
    void updateContent();
};

#endif
