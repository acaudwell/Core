#include "image.h"

UIImage::UIImage(const std::string& image_path)
    : image_path(image_path), coords(0.0f, 0.0f, 1.0f, 1.0f) {

    rect = vec2(0.0f, 0.0f);
    init();
}

UIImage::UIImage(const std::string& image_path, const vec2& rect, const vec4& coords)
    : image_path(image_path), coords(coords) {

    this->rect = rect;

    init();
}

UIImage::~UIImage() {
    if(imagetex != 0) texturemanager.release(imagetex);
}

void UIImage::init() {
    imagetex = texturemanager.grab(image_path);

    colour        = vec4(1.0f);
    shadow_offset = vec2(1.0f, 1.0f);
    shadow        = 0.0f;

    if(glm::length(rect) < 1.0f) {
        rect = vec2(imagetex->w, imagetex->h);
    }
}

void UIImage::setTextureCoords(const vec4& coords) {
    this->coords = coords;
}

void UIImage::drawContent() {
    imagetex->bind();

    if(shadow > 0.0f) {
        glColor4f(0.0f, 0.0f, 0.0f, shadow);
        drawQuad(rect + shadow_offset, coords);
    }

    glColor4fv(glm::value_ptr(colour));
    drawQuad(rect, coords);
}
