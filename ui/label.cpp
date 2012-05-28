#include "label.h"
#include "slider.h"
#include "colour.h"

//UILabel

UILabel::UILabel(const std::string& text, bool editable, float width, UIAction* action) : text(text), width(width), action(action), UIElement() {

    slider       = 0;
    text_colour  = vec4(0.0f);
    bgcolour     = vec4(0.0f);
    text_changed = true;
    expanded     = 0.0f;

    this->editable = editable;

    selected_edit_bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.5f);
    edit_bgcolour          = vec4(0.0f, 0.0f, 0.0f, 0.25f);
}

void UILabel::setWidth(float width) {
    this->width = width;
}

void UILabel::setText(const std::string& text) {
    this->text = text;
    text_changed = true;
}

void UILabel::updateRect() {
    rect.x = width + margin.x + margin.z;
    rect.y = 14.0f + margin.y + margin.w;
}

void UILabel::expandRect(const vec2& expand) {
    expanded = expand.x;
    text_changed = true;
}

void UILabel::resetRect() {
    expanded = 0.0f;
}

void UILabel::drawBackground() {
    if(bgcolour.w <= 0.0f) return;

    glColor4fv(glm::value_ptr(bgcolour));

    ui->setTextured(false);
    drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
    ui->setTextured(true);
}

void UILabel::backspace() {
    if(!text.empty()) {
        text.resize( text.size() - 1 );
    }
}

void UILabel::tab() {
}

bool UILabel::keyPress(SDL_KeyboardEvent *e, char c) {

    if(!c) return false;

    if(!editable) {

        switch(c) {
            case SDLK_RETURN:
                if(submit()) return true;
                break;
        }

        return false;
    }

    // copy / paste clipboard
    if(e->keysym.sym == SDLK_v || e->keysym.sym == SDLK_c) {

        Uint8* keystate = SDL_GetKeyState(0);

        if(keystate[SDLK_LCTRL]) {

            if(e->keysym.sym == SDLK_c) {

                if(!this->text.empty()) {
                    SDLApp::setClipboardText(this->text);
                    return true;
                }

            } else if(e->keysym.sym == SDLK_v) {

                std::string text;
                if(SDLApp::getClipboardText(text)) {
                    setText(this->text + text);
                    return true;
                }
            }

            return false;
        }
    }

    switch(c) {
#ifdef __APPLE__
        case SDLK_DELETE:
#else
	case SDLK_BACKSPACE:
#endif
            backspace();
            break;
        case SDLK_TAB:
            tab();
            break;
        case SDLK_RETURN:
            submit();
            break;
        default:
            text += c;
            break;
    }

    text_changed = true;

    return true;
}

void UILabel::update(float dt) {

    if(selected && editable) {
        cursor_anim += dt;
        if(cursor_anim>=2.0f) cursor_anim=0.0f;
    } else {
        updateContent();
        if(width < 0.0f) {
            width = ui->font.getWidth(text) + 2.0f;
        }

        cursor_anim = 0.0f;
    }

    if(text_changed && width >= 0.0f && ui != 0) {
        display_text = text;
        float text_width = ui->font.getWidth(display_text);

        //add space for cursor
        float text_padding = (editable) ? 10.0f : 0.0f;

        while(text_width+text_padding > (width+expanded) && !display_text.empty()) {
            display_text = display_text.substr(1, display_text.size()-1);
            text_width = ui->font.getWidth(display_text);
        }
        text_changed = false;
    }

    updateRect();
}

UIElement* UILabel::elementAt(const vec2& pos) {

    if(hidden) return 0;

    vec2 rect = getRect() + vec2(expanded, 0.0f);

    if(   pos.x >= this->pos.x && pos.x <= (this->pos.x + rect.x)
       && pos.y >= this->pos.y && pos.y <= (this->pos.y + rect.y)) {
        return this;
    }

    return 0;
}

void UILabel::setTextColour(const vec4& text_colour) {
    this->text_colour = text_colour;
}

void UILabel::idle() {
    if(action != 0) action->idle();
}

void UILabel::drawContent() {

    drawBackground();

    ui->font.alignTop(false);

    vec4 text_colour_alpha;

    if(text_colour.w <= 0.0f) {
        text_colour_alpha = ui->getTextColour();
    } else {
        text_colour_alpha = text_colour * ui->getAlpha();
        if(disabled) text_colour_alpha *= 0.5f;
    }

    ui->font.setColour(text_colour_alpha);

    vec2 rect2 = this->rect;
    rect2.x += expanded;

    if(selected && editable) {

        if(selected_edit_bgcolour.w > 0.0f) {

             ui->setTextured(false);

            glColor4fv(glm::value_ptr(selected_edit_bgcolour));
            drawQuad(rect2, vec4(0.0f, 0.0f, 1.0f, 1.0f));

             ui->setTextured(true);
        }

        if(int(cursor_anim)==0) {
            ui->drawText(margin.x, rect2.y-(3.0+margin.w), "%s_", display_text.c_str());
        } else {
            ui->drawText(margin.x, rect2.y-(3.0+margin.w), display_text);
        }
    } else {
        if(editable && edit_bgcolour.w > 0.0f) {
             ui->setTextured(false);

            glColor4fv(glm::value_ptr(edit_bgcolour));

            drawQuad(rect2, vec4(0.0f, 0.0f, 1.0f, 1.0f));

            ui->setTextured(true);
        }

        ui->drawText(margin.x, rect2.y-(3.0+margin.w), display_text);
    }

    //NOTE: this is the wrong place for this, but it gets the desired result...
//    expanded = 0.0f;
}

//UIIntLabel

UIIntLabel::UIIntLabel(int* value, bool editable, UIAction* action) : value(value), UILabel("", editable, -1.0f, action) {
    width = 80.0f;
    edit_bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

bool UIIntLabel::keyPress(SDL_KeyboardEvent *e, char c) {

    bool changed = UILabel::keyPress(e,c);

    return changed;
}

bool UIIntLabel::submit() {
    int v = atoi(text.c_str());

    if(slider != 0) {
        ((UIIntSlider*)slider)->setValue(v);
    } else {
        *value = v;
    }

    return true;
}

void UIIntLabel::updateContent() {

    char buff[256];
    snprintf(buff, 256, "%d", *value);

    text = std::string(buff);
    text_changed = true;
}

//UIFloatLabel

UIFloatLabel::UIFloatLabel(UIFloatSlider* slider, bool editable, UIAction* action) : value(slider->value),  UILabel("", editable, -1.0f, action) {
    this->slider = slider;
    width = 80.0f;
    edit_bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

UIFloatLabel::UIFloatLabel(float* value, bool editable, UIAction* action) : value(value), UILabel("", editable, -1.0f, action) {
    width = 80.0f;
    edit_bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

bool UIFloatLabel::keyPress(SDL_KeyboardEvent *e, char c) {

    bool changed = UILabel::keyPress(e,c);

    return changed;
}

void UIFloatLabel::setValue(float* value) {
    this->value = value;
}

bool UIFloatLabel::submit() {

    float v = atof(text.c_str());

    if(slider != 0) {
        ((UIFloatSlider*)slider)->setValue(v);
    } else {
        *value = v;
        if(action != 0) action->perform();
    }

    return true;
}

void UIFloatLabel::setSelected(bool selected) {
    UILabel::setSelected(selected);
    if(selected && editable) updateContent();
}

void UIFloatLabel::updateContent() {
    if(!value) {
        if(!text.empty()) {
            text.resize(0);
            text_changed = true;
        }
        return;
    }

    char buff[256];
    snprintf(buff, 256, "%f", *value);
    text = std::string(buff);
    text_changed = true;

    //trim trailing zeros - ideally we should only do this when the value changes
    size_t dotsep = text.rfind(".");
    size_t tlen   = text.size();

    size_t dotstop = selected ? dotsep-1 : dotsep+1;

    if(tlen>1 && dotsep != std::string::npos && dotsep != tlen-1) {

        size_t zpos = tlen-1;

        while(zpos>dotstop && (text[zpos] == '0' || text[zpos] == '.')) zpos--;

        if(zpos<tlen-1) text.resize(zpos+1);
    }
}

UIColourLabel::UIColourLabel(UIColourSlider* slider, bool editable) : UIFloatLabel((float*)0, editable) {
    this->slider = slider;
    width = 80.0f;
    edit_bgcolour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

bool UIColourLabel::submit() {

    float v = atof(text.c_str());

    ((UIColourSlider*)slider)->setValue(v);

    return true;
}

void UIColourLabel::updateContent() {
    value = ((UIColourSlider*)slider)->attribute;
    UIFloatLabel::updateContent();
}
