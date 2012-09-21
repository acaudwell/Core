#ifndef UI_IMAGE_H
#define UI_IMAGE_H

#include "element.h"

class UIImage : public UIElement {
    TextureResource* imagetex;
public:
    std::string image_path;
    vec4  coords;
    float shadow;
    vec2  shadow_offset;
    vec4  colour;

    UIImage(const std::string& image_path);
    UIImage(const std::string& image_path, const vec2& rect, const vec4& coords);

    void setTextureCoords(const vec4& coords);

    void init();

    ~UIImage();

    int getType() const { return UI_IMAGE; };

    void drawContent();
};

#endif
