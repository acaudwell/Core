#include "file_selector.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

UIFileSelector::UIFileSelector(const std::string& title, const std::string& dir, UIFileSelectorAction* action)
    : action(action), UIGroup(title, false, true) {

    minimizable = false;

    listing = new UIScrollLayout(vec2(500.0f, 150.0f));
    listing->setDrawBackground(true);
    listing->bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.25f);
    listing->setFill(true);

    std::string initial_dir = dir;

    // remove trailing slash
    if(!initial_dir.size() > 2 && initial_dir[1] != ':' && (initial_dir[initial_dir.size()-1] == '/' || initial_dir[initial_dir.size()-1] == '\\')) {
        initial_dir.resize(dir.size() - 1);
    }

    dir_path = new UIDirInputLabel(this, "");
    dir_path->setFillHorizontal(true);

    file_path = new UIFileInputLabel(this, "");
    file_path->setFillHorizontal(true);

    filter_select = new UISelect();

    layout->addElement(new UILabelledElement("Path",  dir_path,  120.0f));
    layout->addElement(listing);
    layout->addElement(new UILabelledElement("Name",  file_path, 120.0f));
    layout->addElement(new UILabelledElement("Filter", filter_select, 120.0f));

    addFilter("All Files (*.*)", "");

    current_filter = 0;

    changeDir(initial_dir);
}

bool _listing_sort (const boost::filesystem::path& a,const boost::filesystem::path& b) {
    bool dir_a = is_directory(a);
    bool dir_b = is_directory(b);

    if(dir_a != dir_b) return dir_b < dir_a;

    return boost::ilexicographical_compare(a.filename().string(), b.filename().string());
}

void UIFileSelector::addFilter(const std::string& name, const std::string& extension, bool select) {
    UIOptionLabel* option = filter_select->addOption(name, extension);
    if(select) filter_select->selectOption(option, false);
}

//standardize directory string appearance
void UIFileSelector::prettyDirectory(std::string& dir_string) {
    if(!dir_string.empty() && (dir_string[dir_string.size()-1] != '\\' && dir_string[dir_string.size()-1] != '/')) {
        dir_string += "/";
    }

#ifdef _WIN32
    std::replace(dir_string.begin(),dir_string.end(),'/','\\');
#else
    std::replace(dir_string.begin(),dir_string.end(),'\\','/');
#endif
}

bool UIFileSelector::changeDir(const std::string& dir) {

    boost::filesystem::path path(dir);

    if(!is_directory(path)) return false;

    next_dir = path.string();

    return true;
}

void UIFileSelector::update(float dt) {

    UIOptionLabel* selected_filter = filter_select->getSelectedOption();

    if(current_filter != selected_filter || !next_dir.empty()) {
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
    if(ui!=0) ui->selectElement(file_path);
}

void UIFileSelector::close() {
    filter_select->open = false;
    hidden=true;
}

void UIFileSelector::selectFile(const std::string& path) {
    selected_path = path;
}

void UIFileSelector::selectPath(const std::string& select_path) {

    this->selected_path = select_path;

    boost::filesystem::path path(selected_path);

    if(!is_directory(path)) {
        file_path->setText(path.filename().string());
    } else {
        file_path->setText("");
    }
}

const std::string& UIFileSelector::getCurrentDir() const {
    return current_dir;
}

void UIFileSelector::confirm() {
    action->perform(selected_path);
}

//return true and set parent_path if there is one
//handle boosts unhelpful treatment of trailing slash on directories
bool parentPath(const boost::filesystem::path& path, boost::filesystem::path& parent_path) {

    parent_path = path.parent_path();

    if(path.filename().string() == ".") {
        parent_path = parent_path.parent_path();
    }

    if(is_directory(parent_path)) return true;

    return false;
}

void UIFileSelector::updateListing() {

    if(!next_dir.empty()) {
        prettyDirectory(next_dir);

        previous_dir = dir_path->text;

        dir_path->setText(next_dir);

        next_dir.resize(0);
    }

    if(dir_path->text.empty()) return;

    boost::filesystem::path p(dir_path->text);

    if(!is_directory(p)) return;

    std::vector<boost::filesystem::path> dir_listing;

    try {

        copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(dir_listing));
        std::sort(dir_listing.begin(), dir_listing.end(), _listing_sort);

    } catch(const boost::filesystem::filesystem_error& exception) {

        //switch to previous directory if there is one
        if(!previous_dir.empty() && is_directory(boost::filesystem::path(previous_dir))) {
            dir_path->setText(previous_dir);
            previous_dir = "";
        }

        return;
    }

    current_dir = p.string();

    listing->setUI(ui);
    listing->clear();

    //add .. if there is a parent directory

    boost::filesystem::path parent_path;

    if(parentPath(p, parent_path) && !(parent_path.string().size() == 2 && parent_path.string()[1] == ':')) {
        listing->addElement(new UIFileSelectorLabel(this, "..", parent_path.string()));
    }

    for(const boost::filesystem::path& l: dir_listing) {

        std::string filename(l.filename().string());

        if(filename.empty()) continue;

#ifdef _WIN32
        DWORD win32_attr = GetFileAttributes(l.string().c_str());
        if (win32_attr & FILE_ATTRIBUTE_HIDDEN || win32_attr & FILE_ATTRIBUTE_SYSTEM) continue;
#else
        if(filename[0] == '.') continue;
#endif

        boost::algorithm::to_lower(filename);

        if(current_filter != 0 && !current_filter->value.empty() && !is_directory(l)) {
            size_t at = filename.rfind(current_filter->value);

            if(at == std::string::npos || at != (filename.size() - current_filter->value.size()))
                continue;
        }

        UIFileSelectorLabel* file_label = new UIFileSelectorLabel(this, l.string());
        file_label->setFillHorizontal(true);

        listing->addElement(file_label);
    }

    listing->update(0.1f);

    listing->horizontal_scrollbar->scrollToStart();
    listing->vertical_scrollbar->scrollToStart();

    listing->vertical_scrollbar->bar_step = 1.0f / listing->getElementCount();
}

std::string UIFileSelector::autocomplete(const std::string& input, bool dirs_only) {

    //find all files/dirs prefixed by input
    //find minimum set of common characters to expand input by

    std::string result;

    //normalize
    boost::filesystem::path input_path(input);

    boost::filesystem::path parent_path;

    if(parentPath(input_path, parent_path)) {

        try {
            std::vector<boost::filesystem::path> dir_listing;
            copy(boost::filesystem::directory_iterator(parent_path), boost::filesystem::directory_iterator(), back_inserter(dir_listing));

            std::string input_path_string = input_path.string();

            size_t input_path_size = input_path_string.size();

            for(boost::filesystem::path& l: dir_listing) {

                if(dirs_only && !is_directory(l)) continue;

                std::string path_string = l.string();
                std::string filename(l.filename().string());

                if(filename.empty()) continue;
#ifdef _WIN32
                DWORD win32_attr = GetFileAttributes(l.string().c_str());
                if (win32_attr & FILE_ATTRIBUTE_HIDDEN || win32_attr & FILE_ATTRIBUTE_SYSTEM) continue;
#else
                if(filename[0] == '.') continue;
#endif

                //check if input is a prefix of this string
                if(path_string.find(input_path_string) == 0) {

                    if(result.empty()) result = path_string;
                    else {
                        //find minimum subset of current match longer than input that
                        //is also a prefix of this string
                        while(!result.empty() && result.size() > input_path_size) {
                            if(path_string.find(result) == 0) {
                                break;
                            }
                            result.resize(result.size()-1);
                        }
                    }
                }
            }

        } catch(const boost::filesystem::filesystem_error& exception) {
            result = input;
        }
    }

    if(result.size() < input.size()) result = input;

    if(is_directory(boost::filesystem::path(result))) {
        prettyDirectory(result);
    }

    return result;
}

//UIFileSelectorLabel

UIFileSelectorLabel::UIFileSelectorLabel(UIFileSelector* selector, const std::string& label, const std::string& path)
    : selector(selector), path(path), UILabel(label, false, 420.0f) {
    directory = is_directory(boost::filesystem::path(path));
    selectable = true;
}

UIFileSelectorLabel::UIFileSelectorLabel(UIFileSelector* selector, const std::string& path)
    : selector(selector), path(path), UILabel(boost::filesystem::path(path).filename().string(), false, 420.0f) {
    directory = is_directory(boost::filesystem::path(path));
    selectable = true;
}

void UIFileSelectorLabel::updateContent() {
    text_colour = selected  ? vec4(1.0f) :
                  directory ? vec4(0.0f, 1.0f, 1.0f, 1.0f) : vec4(0.0f, 1.0f, 0.0f, 1.0f);
    bgcolour = selected ? vec4(1.0f, 1.0f, 1.0f, 0.15f) : vec4(0.0f);
}

void UIFileSelectorLabel::click(const vec2& pos) {
    if(!directory) selector->selectPath(path);
}

void UIFileSelectorLabel::doubleClick(const vec2& pos) {
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

//UIDirInputLabel
UIDirInputLabel::UIDirInputLabel(UIFileSelector* selector, const std::string& dirname)
    : selector(selector), UILabel(dirname, true, 300.0f) {
}


bool UIDirInputLabel::submit() {
    selector->changeDir(text);
    return true;
}

void UIDirInputLabel::tab() {
    setText(selector->autocomplete(text, true));
}

//UIFileInputLabel
UIFileInputLabel::UIFileInputLabel(UIFileSelector* selector, const std::string& filename)
    : selector(selector), UILabel(filename, true,  300.0f) {
}

void UIFileInputLabel::tab() {
    boost::filesystem::path filepath(text);

    if(!is_directory(filepath.parent_path())) {
        filepath = selector->getCurrentDir() / filepath;
        if(!exists(filepath.parent_path())) return;

        std::string completed = selector->autocomplete(filepath.string());

        if(completed != filepath.string()) {

            std::string cur_dir = selector->getCurrentDir();

            size_t curr_len = selector->getCurrentDir().size();

            if(completed.size() > curr_len) {
                completed = completed.substr(curr_len);
            }

            setText(boost::filesystem::path(completed).string());
        }
    }

    setText(selector->autocomplete(text));
}

bool UIFileInputLabel::submit() {

    boost::filesystem::path filepath(text);

    if(!exists(filepath) || filepath.is_relative()) {
        filepath =  boost::filesystem::path(selector->getCurrentDir()) / filepath;
        if(!exists(filepath)) return false;
    }

    //TODO: construct full path here first

    if(is_directory(filepath)) {
        selector->changeDir(filepath.string());
        setText("");
        return true;
    }

    selector->selectFile(filepath.string());
    selector->confirm();
    return true;
}
