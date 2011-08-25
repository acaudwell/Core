#include "fileselector.h"

UIFileSelector::UIFileSelector(const std::string& title, const std::string& dir, const std::string& file) : UIGroup(title, true) {
    dir_path  = new UILabel(dir, true, false, 400.0f);
    listing   = new UILayout();
    file_path = new UILabel(file, true, false, 400.0f);   
    
    for(int i=0;i<10;i++) {
        listing->addElement(new UILabel("x"));
    }
    
    
    layout->addElement(new UILabelledElement("Path",  dir_path,  120.0f));
    layout->addElement(new UILabelledElement("",      listing,   120.0f));
    layout->addElement(new UILabelledElement("Name",  file_path, 120.0f));

    updateListing();
}

void UIFileSelector::updateListing() {
    listing->clear();
}
