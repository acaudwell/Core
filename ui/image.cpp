#include "image.h"

UIImage::UIImage(const std::string& path) {
    imagetex = texturemanager.grab(path);
    shadow_offset = vec2f(1.0f, 1.0f);
    shadow = 0.0f;
    alpha = 1.0f;
}

UIImage::~UIImage() {
    if(imagetex != 0) texturemanager.release(imagetex);
}

void UIImage::updateRect() {
    rect.x = imagetex->w;
    rect.y = imagetex->h;
}

void UIImage::drawContent() {
    imagetex->bind();

    if(shadow > 0.0f) {
        glColor4f(0.0f, 0.0f, 0.0f, shadow);
        drawQuad(rect + shadow_offset, vec4f(0.0f, 0.0f, 1.0f, 1.0f));
    }

    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    drawQuad(rect, vec4f(0.0f, 0.0f, 1.0f, 1.0f));
}
