#include "fileselector.h"

UIFileSelector::UIFileSelector(const std::string& title, const std::string& dir, const std::string& file) : UIGroup(title, true) {
    dir_path  = new UILabel(dir, true, false, 400.0f);
    listing   = new UIScrollLayout(vec2(520.0f, 100.0f));
    file_path = new UILabel(file, true, false, 400.0f);

//    for(int i=0;i<10;i++) {
//        listing->addElement(new UILabel("x"));
//    }

    listing->background = vec4(0.0, 0.0, 0.0, 0.3);

    layout->addElement(new UILabelledElement("Path",  dir_path,  120.0f));
    layout->addElement(listing);
    layout->addElement(new UILabelledElement("Name",  file_path, 120.0f));

    updateListing();
}

void UIFileSelector::updateListing() {
    listing->clear();

    if(dir_path->text.empty()) return;

    boost::filesystem::path p(dir_path->text);

    if(!is_directory(p)) return;


    std::vector<boost::filesystem::path> dir_listing;
    copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(dir_listing));

    foreach(boost::filesystem::path l, dir_listing) {
        listing->addElement(new UIFileSelectorLabel(l));
    }
}

//UIFileSelectorLabel

UIFileSelectorLabel::UIFileSelectorLabel(const boost::filesystem::path& path) : path(path), UILabel(path.filename().string(), false, false, 520.0f) {   
    font_colour = is_directory(path) ? vec3(0.5f, 0.5f, 1.0f) : vec3(0.5f, 1.0f, 0.5f);
}

//UIDirPathLabel

//UIFilePathLabel

