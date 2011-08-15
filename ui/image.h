#ifndef UI_IMAGE_H

#include "element.h"

class UIImage : public UIElement {
    TextureResource* imagetex;
public:
    float alpha;
    float shadow;
    vec2  shadow_offset;

    UIImage(const std::string& path);
    ~UIImage();

    int getType() { return UI_IMAGE; }

    void updateRect();
    void drawContent();
};

#endif
