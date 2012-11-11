#include "solid_layout.h"

UISolidLayout::UISolidLayout(bool horizontal) : UILayout(horizontal) {

    inverted   = false;

    bgtex.resize(4);

    bgtex[0] = texturemanager.grab("ui/layout_tl.png", false);
    bgtex[1] = texturemanager.grab("ui/layout_tr.png", false);
    bgtex[2] = texturemanager.grab("ui/layout_br.png", false);
    bgtex[3] = texturemanager.grab("ui/layout_bl.png", false);

    for(TextureResource* t: bgtex) {
        t->bind();
        t->setFiltering(GL_NEAREST, GL_NEAREST);
        t->setWrapStyle(GL_CLAMP);
    }
}

UISolidLayout::~UISolidLayout() {
    for(TextureResource* t: bgtex) {
        texturemanager.release(t);
    }
    bgtex.clear();
}

void UISolidLayout::drawBackground() {

    glPushMatrix();

    glTranslatef(pos.x, pos.y, 0.0f);

    glColor4fv(glm::value_ptr(ui->getSolidColour()));

    ui->setTextured(true);

    vec4 texcoord;

    vec2 rect = getRect();

    for(int i=0; i < 4; i++) {

        glPushMatrix();

        if(inverted) {
            bgtex[(i+2)%4]->bind();

            switch(i) {
                case 0:
                    texcoord = vec4(1.0f, 1.0f, 1.0-(rect.x/32.0f), 1.0-(rect.y/32.0f));
                    break;
                case 1:
                    texcoord = vec4((rect.x/32.0f), 1.0f, 0.0, 1.0-(rect.y/32.0f));
                    glTranslatef(rect.x*0.5, 0.0f, 0.0f);
                    break;
                case 2:
                    texcoord = vec4(rect.x/32.0f, rect.y/32.0f, 0.0f, 0.0f);
                    glTranslatef(rect.x*0.5, rect.y*0.5, 0.0f);
                    break;
                case 3:
                    texcoord = vec4(1.0f, (rect.y/32.0f), 1.0f-(rect.x/32.0f), 0.0f);
                    glTranslatef(0.0f, rect.y*0.5, 0.0f);
                    break;
            }
        } else {
            bgtex[i]->bind();

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

    glPopMatrix();
}
