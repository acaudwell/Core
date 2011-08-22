#include "label.h"
#include "slider.h"

//UILabel

UILabel::UILabel(const std::string& text, bool editable, bool opaque, float width) : text(text), editable(editable), opaque(opaque), width(width), UIElement() {

    if(opaque) {
        labeltex.resize(4);

        labeltex[0] = texturemanager.grab("ui/label_tl.png", false);
        labeltex[1] = texturemanager.grab("ui/label_tr.png", false);
        labeltex[2] = texturemanager.grab("ui/label_br.png", false);
        labeltex[3] = texturemanager.grab("ui/label_bl.png", false);

        foreach(TextureResource* t, labeltex) {
            t->bind();
            t->setFiltering(GL_NEAREST, GL_NEAREST);
            t->setWrapStyle(GL_CLAMP);
        }
    }
    slider = 0;
}

UILabel::~UILabel() {
    foreach(TextureResource* t, labeltex) {
        texturemanager.release(t);
    }
    labeltex.clear();
}

void UILabel::setWidth(float width) {
    this->width = width;
}

void UILabel::setText(const std::string& text) {
    this->text = text;
    width = ui->font.getWidth(text);
}

void UILabel::updateRect() {
    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
}

void UILabel::drawBackground() {

    vec4 texcoord;

    for(int i=0;i<4;i++) {
        labeltex[i]->bind();

        glPushMatrix();

            switch(i) {
                case 0:
                    texcoord = vec4(0.0f, 0.0f, rect.x/32.0f, rect.y/32.0f);
                    break;
                case 1:
                    texcoord = vec4(1.0f-(rect.x/32.0f), 0.0f, 1.0f, (rect.y/32.0f));
                    glTranslatef(rect.x*0.5, 0.0f, 0.0f);
                    break;
                case 2:
                    texcoord = vec4(1.0-rect.x/32.0f, 1.0-rect.y/32.0f, 1.0f, 1.0f);
                    glTranslatef(rect.x*0.5, rect.y*0.5, 0.0f);
                    break;
                case 3:
                    texcoord = vec4(0.0, 1.0-(rect.y/32.0f), (rect.x/32.0f), 1.0f);;
                    glTranslatef(0.0f, rect.y*0.5, 0.0f);
                    break;
            }

            glBegin(GL_QUADS);
                glTexCoord2f(texcoord.x,texcoord.y);
                glVertex2f(0.0f,    0.0f);

                glTexCoord2f(texcoord.z,texcoord.y);
                glVertex2f(rect.x*0.5,  0.0f);

                glTexCoord2f(texcoord.z,texcoord.w);
                glVertex2f(rect.x*0.5, rect.y*0.5);

                glTexCoord2f(texcoord.x,texcoord.w);
                glVertex2f(0.0f,   rect.y*0.5);
            glEnd();

        glPopMatrix();
    }

}

bool UILabel::keyPress(SDL_KeyboardEvent *e, char c) {
    if(!editable) return false;

    switch(c) {
        case SDLK_BACKSPACE:
            if(!text.empty()) {
                text.resize( text.size() - 1 );
            }
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

    if(opaque) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        drawBackground();
    }

    ui->font.alignTop(false);

    if(selected && editable) {
        glDisable(GL_TEXTURE_2D);

        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));

        glEnable(GL_TEXTURE_2D);

        if(!disabled) {
            ui->font.setColour(vec4(1.0, 1.0, 1.0, 1.0f));
        } else {
            ui->font.setColour(vec4(0.5, 0.5, 0.5, 1.0f));
        }

        if(int(cursor_anim)==0) {
            ui->font.print(margin.x, rect.y-(3.0+margin.y), "%s_", text.c_str());
        } else {
            ui->font.draw(margin.x, rect.y-(3.0+margin.y), text);
        }
    } else {
        ui->font.draw(margin.x, rect.y-(3.0+margin.y), text);
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

void UIIntLabel::updateRect() {

    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
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

void UIFloatLabel::updateRect() {

    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
}
