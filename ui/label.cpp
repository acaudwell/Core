#include "label.h"
#include "slider.h"

//UILabel

UILabel::UILabel(const std::string& text, bool editable, float width) : text(text), editable(editable), width(width), UIElement() {

    slider      = 0;
    font_colour = vec3(1.0f);
    bgcolour    = vec4(0.0f);
}

void UILabel::setWidth(float width) {
    this->width = width;
}

void UILabel::setText(const std::string& text) {
    this->text = text;
}

void UILabel::updateRect() {
    rect.x = width + margin.x + margin.z;
    rect.y = 14.0f + margin.y + margin.w;
}

void UILabel::drawBackground() {
    if(bgcolour.w <= 0.0f) return;

    glColor4fv(glm::value_ptr(bgcolour));

    glDisable(GL_TEXTURE_2D);
    drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glEnable(GL_TEXTURE_2D);
}

bool UILabel::keyPress(SDL_KeyboardEvent *e, char c) {
    if(!editable) {

        switch(c) {
            case SDLK_RETURN:
                submit();
                return true;
                break;
        }
                
        return false;
    }
    
    
    switch(c) {
        case SDLK_BACKSPACE:
            if(!text.empty()) {
                text.resize( text.size() - 1 );
            }
            break;
        case SDLK_RETURN:
            submit();
            break;
        default:
            text += c;
            break;
    }

    return true;
}

void UILabel::update(float dt) {

    if(selected && editable) {
        cursor_anim += dt;
        if(cursor_anim>=2.0f) cursor_anim=0.0f;
    } else {
        updateContent();
        if(width < 0.0f) width = ui->font.getWidth(text);
        cursor_anim = 0.0f;
    }

    updateRect();
}

void UILabel::drawContent() {

    drawBackground();

    ui->font.alignTop(false);

    if(!disabled) {
        ui->font.setColour(vec4(font_colour, 1.0f));
    } else {
        ui->font.setColour(vec4(font_colour*0.5f, 1.0f));
    }

    if(selected && editable) {
        glDisable(GL_TEXTURE_2D);

        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));

        glEnable(GL_TEXTURE_2D);

        if(int(cursor_anim)==0) {
            ui->font.print(margin.x, rect.y-(3.0+margin.w), "%s_", text.c_str());
        } else {
            ui->font.draw(margin.x, rect.y-(3.0+margin.w), text);
        }
    } else {
        ui->font.draw(margin.x, rect.y-(3.0+margin.w), text);
    }
}

//UIIntLabel

UIIntLabel::UIIntLabel(int* value, bool editable) : value(value), UILabel("", editable) {
    width = 80.0f;
}

bool UIIntLabel::keyPress(SDL_KeyboardEvent *e, char c) {

    bool changed = UILabel::keyPress(e,c);

    if(changed) {
        int v = atoi(text.c_str());

        if(slider != 0) {
            ((UIIntSlider*)slider)->setValue(v);
        } else {
            *value = v;
        }
    }

    return changed;
}

void UIIntLabel::updateContent() {

    char buff[256];
    snprintf(buff, 256, "%d", *value);

    text = std::string(buff);
}

//UIFloatLabel

UIFloatLabel::UIFloatLabel(float* value, bool editable) : value(value), UILabel("", editable) {
    width = 80.0f;
}

bool UIFloatLabel::keyPress(SDL_KeyboardEvent *e, char c) {

    bool changed = UILabel::keyPress(e,c);

    if(changed) {

        float v = atof(text.c_str());

        if(slider != 0) {
            ((UIFloatSlider*)slider)->setValue(v);
        } else {
            *value = v;
        }
    }

    return changed;
}

void UIFloatLabel::updateContent() {
    char buff[256];
    snprintf(buff, 256, "%.5f", *value);
    text = std::string(buff);

    //trim trailing zeros - ideally we should only do this when the value changes
    size_t dotsep = text.rfind(".");
    size_t tlen    = text.size();

    if(tlen>1 && dotsep != std::string::npos && dotsep != tlen-1) {

        size_t zpos = tlen-1;

        while(zpos>dotsep+1 && text[zpos] == '0') zpos--;

        if(zpos<tlen-1) text.resize(zpos+1);
    }
}

