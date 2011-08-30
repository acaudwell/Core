#include "fileselector.h"

UIFileSelector::UIFileSelector(const std::string& title, const std::string& dir, const std::string& file) : UIGroup(title, true) {
    dir_path  = new UILabel(dir, true, false, 400.0f);
    listing   = new UIScrollLayout(vec2(520.0f, 100.0f));
    file_path = new UILabel(file, true, false, 400.0f);

//    for(int i=0;i<10;i++) {
//        listing->addElement(new UILabel("x"));
//    }

    //listing->background = vec4(0.6, 0.6, 0.6, 1.0);

    layout->addElement(new UILabelledElement("Path",  dir_path,  120.0f));
    layout->addElement(listing);
    layout->addElement(new UILabelledElement("Name",  file_path, 120.0f));

    updateListing();
}

bool _listing_sort (const boost::filesystem::path& a,const boost::filesystem::path& b) {
    bool dir_a = is_directory(a);
    bool dir_b = is_directory(b);

    if(dir_a != dir_b) return dir_b < dir_a;
        
    return boost::ilexicographical_compare(a.filename().string(), b.filename().string());
}

void UIFileSelector::updateListing() {
    listing->clear();

    if(dir_path->text.empty()) return;

    boost::filesystem::path p(dir_path->text);

    if(!is_directory(p)) return;

    std::vector<boost::filesystem::path> dir_listing;
    copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(dir_listing));

    std::sort(dir_listing.begin(), dir_listing.end(), _listing_sort);
    
    foreach(boost::filesystem::path l, dir_listing) {
        if(!l.filename().string().empty() && l.filename().string()[0] != '.') 
        listing->addElement(new UIFileSelectorLabel(l));
    }
    
    listing->update(0.1f);
    
    listing->vertical_scrollbar->bar_step = 1.0f / listing->getElementCount();
}

//UIFileSelectorLabel

UIFileSelectorLabel::UIFileSelectorLabel(const boost::filesystem::path& path) : path(path), UILabel(path.filename().string(), false, false, 520.0f) {   
    font_colour = is_directory(path) ? vec3(0.0f, 1.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);
}

//UIDirPathLabel

//UIFilePathLabel

