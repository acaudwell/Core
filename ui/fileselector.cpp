#include "fileselector.h"

UIFileSelector::UIFileSelector(const std::string& title, const std::string& dir) : UIGroup(title, true) {

    minimizable = false;
    
    listing   = new UIScrollLayout(vec2(520.0f, 100.0f));

    dir_path  = new UIDirSelectLabel(this, dir);
    file_path = new UIFileSelectLabel(this, "");

    filter_select = new UISelect();
    
    layout->addElement(new UILabelledElement("Path",  dir_path,  120.0f));
    layout->addElement(listing);
    layout->addElement(new UILabelledElement("Name",  file_path, 120.0f));
    layout->addElement(new UILabelledElement("Filter", filter_select, 120.0f));

    addFilter("All Files (*.*)", "");
    
    current_filter = filter_select->getSelectedOption();
    
    updateListing();
}

bool _listing_sort (const boost::filesystem::path& a,const boost::filesystem::path& b) {
    bool dir_a = is_directory(a);
    bool dir_b = is_directory(b);

    if(dir_a != dir_b) return dir_b < dir_a;

    return boost::ilexicographical_compare(a.filename().string(), b.filename().string());
}

void UIFileSelector::addFilter(const std::string& name, const std::string& extension) {
    filter_select->addOption(name, extension);
}

bool UIFileSelector::changeDir(const boost::filesystem::path& dir) {

    if(!is_directory(dir)) return false;

    dir_path->setText(dir.string());

    updateListing();

    return true;
}

void UIFileSelector::update(float dt) {
        
    UIOptionLabel* selected_filter = filter_select->getSelectedOption();
    
    if(current_filter != selected_filter) {
        current_filter = selected_filter;
        updateListing();
    }

    UIGroup::update(dt);
}

void UIFileSelector::toggle() {
    if(hidden) open();
    else close();
}

void UIFileSelector::open() {
    hidden=false;
    updateListing();
}

void UIFileSelector::close() {
    filter_select->open = false;
    hidden=true;
}

void UIFileSelector::selectPath(const boost::filesystem::path& path) {
    this->selected_path = path;
    
    if(!is_directory(selected_path)) {
        file_path->setText(selected_path.filename().string());
    } else {
        file_path->setText("");
    }
}

void UIFileSelector::confirm() {
}

void UIFileSelector::updateListing() {

    if(dir_path->text.empty()) return;

    boost::filesystem::path p(dir_path->text);

    if(!is_directory(p)) return;

    std::vector<boost::filesystem::path> dir_listing;

    try {
        copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(dir_listing));
    } catch(const boost::filesystem::filesystem_error& exception) {
        // TODO: do something here?
        return;
    }
    listing->clear();

    std::sort(dir_listing.begin(), dir_listing.end(), _listing_sort);

    if(is_directory(p.parent_path())) {        
        listing->addElement(new UIFileSelectorLabel(this, "..", p.parent_path()));
    }

    foreach(boost::filesystem::path l, dir_listing) {

        std::string filename(l.filename().string());

        if(filename.empty()) continue;
                
#ifdef _WIN32
        DWORD win32_attr = GetFileAttributes(l.string().c_str());
        if (win32_attr & FILE_ATTRIBUTE_HIDDEN || win32_attr & FILE_ATTRIBUTE_SYSTEM) continue;
#else
        if(filename[0] == '.') continue;
#endif

        if(current_filter != 0 && !current_filter->value.empty() && !is_directory(l)) {
            size_t at = filename.rfind(current_filter->value);
            
            if(at == std::string::npos || at != (filename.size() - current_filter->value.size()))
                continue;
        }

        listing->addElement(new UIFileSelectorLabel(this, l));
    }

    listing->update(0.1f);

    listing->horizontal_scrollbar->reset();
    listing->vertical_scrollbar->reset();

    listing->vertical_scrollbar->bar_step = 1.0f / listing->getElementCount();
}

//UIFileSelectorLabel

UIFileSelectorLabel::UIFileSelectorLabel(UIFileSelector* selector, const std::string& label, const boost::filesystem::path& path)
    : selector(selector), path(path), UILabel(label, false, 520.0f) {
    directory = is_directory(path);
}

UIFileSelectorLabel::UIFileSelectorLabel(UIFileSelector* selector, const boost::filesystem::path& path)
    : selector(selector), path(path), UILabel(path.filename().string(), false, 520.0f) {
    directory = is_directory(path);
}

void UIFileSelectorLabel::updateContent() {
    font_colour = selected  ? vec3(1.0f) :
                  directory ? vec3(0.0f, 1.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);
    bgcolour = selected ? vec4(1.0f, 1.0f, 1.0f, 0.15f) : vec4(0.0f);
}

void UIFileSelectorLabel::click() {
    if(!directory) selector->selectPath(path);
}

void UIFileSelectorLabel::doubleClick() {
    submit();
}

bool UIFileSelectorLabel::submit() {

    if(directory) {
        ui->deselect();
        selector->changeDir(path);
    } else {
        selector->selectPath(path);
        selector->confirm();
    }
    
    return true;
}

//UIDirSelectLabel
UIDirSelectLabel::UIDirSelectLabel(UIFileSelector* selector, const std::string& dirname)
    : selector(selector), UILabel(dirname, true, 400.0f) {
}


bool UIDirSelectLabel::submit() {
    selector->changeDir(text);
    return true;
}

//UIFileSelectLabel
UIFileSelectLabel::UIFileSelectLabel(UIFileSelector* selector, const std::string& filename)
    : selector(selector), UILabel(filename, true,  400.0f) {
}

bool UIFileSelectLabel::submit() {
    selector->confirm();
    return true;
}