#include "label.h"

//UILabel

UILabel::UILabel(const std::string& text, bool editable, bool opaque) : text(text), editable(editable), opaque(opaque), UIElement() {

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
    width = 120.0f;
}

UILabel::~UILabel() {
    foreach(TextureResource* t, labeltex) {
        texturemanager.release(t);
    }
    labeltex.clear();
}

void UILabel::setText(const std::string& text) {
    this->text = text;
}

void UILabel::updateRect() {
    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
}

void UILabel::drawBackground() {

    vec4f texcoord;

    for(int i=0;i<4;i++) {
        labeltex[i]->bind();

        glPushMatrix();

            switch(i) {
                case 0:
                    texcoord = vec4f(0.0f, 0.0f, rect.x/32.0f, rect.y/32.0f);
                    break;
                case 1:
                    texcoord = vec4f(1.0f-(rect.x/32.0f), 0.0f, 1.0f, (rect.y/32.0f));
                    glTranslatef(rect.x*0.5, 0.0f, 0.0f);
                    break;
                case 2:
                    texcoord = vec4f(1.0-rect.x/32.0f, 1.0-rect.y/32.0f, 1.0f, 1.0f);
                    glTranslatef(rect.x*0.5, rect.y*0.5, 0.0f);
                    break;
                case 3:
                    texcoord = vec4f(0.0, 1.0-(rect.y/32.0f), (rect.x/32.0f), 1.0f);;
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

void UILabel::drawContent() {

    if(opaque) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        drawBackground();
    }

    ui->font.alignTop(false);

    if(selected && editable) {
        glDisable(GL_TEXTURE_2D);

        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        drawQuad(rect, vec4f(0.0f, 0.0f, 1.0f, 1.0f));

        glEnable(GL_TEXTURE_2D);

        if(!disabled) {
            ui->font.setColour(vec4f(1.0, 1.0, 1.0, 1.0f));
        } else {
            ui->font.setColour(vec4f(0.5, 0.5, 0.5, 1.0f));
        }

        ui->font.print(margin.x, rect.y-(3.0+margin.y), "%s_", text.c_str());
    } else {
        ui->font.draw(margin.x, rect.y-(3.0+margin.y), text);
    }
}

//UIIntLabel

UIIntLabel::UIIntLabel(int* value, bool editable) : value(value), UILabel("", editable) {
    width = 80.0f;
}

void UIIntLabel::updateRect() {

    char buff[256];
    snprintf(buff, 256, "%d", *value);

    text = std::string(buff);

    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
}

//UIFloatLabel

UIFloatLabel::UIFloatLabel(float* value, bool editable) : value(value), UILabel("", editable) {
    width = 80.0f;
}

void UIFloatLabel::updateRect() {

    char buff[256];
    snprintf(buff, 256, "%.5f", *value);

    text = std::string(buff);

    rect.x = width + margin.x*2.0f;
    rect.y = 14.0f + margin.y*2.0f;
}
