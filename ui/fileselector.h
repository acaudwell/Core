#ifndef UI_FILE_SELECTOR_H
#define UI_FILE_SELECTOR_H

#include "group.h"
#include "scroll_layout.h"

#include <boost/filesystem.hpp>

class UIFileSelectorLabel : public UILabel {
protected:
    boost::filesystem::path path;
public:
    UIFileSelectorLabel(const boost::filesystem::path& path);
};

class UIFileSelector : public UIGroup {

    UILabel* dir_path;
    UILabel* file_path;
    UILayout* listing;
public:
    UIFileSelector(const std::string& title, const std::string& dir, const std::string& file);
    
    void updateListing();
};

#endif
