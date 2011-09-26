#include "layout.h"

UILayout::UILayout(bool horizontal) : horizontal(horizontal), UIElement() {
    alignment = UI_LAYOUT_ALIGN_NONE;
    bgcolour  = vec4(0.0f);
    min_rect  = vec2(0.0f);
    centre    = false;
}

UILayout::~UILayout() {
    clear();
}

void UILayout::clear() {
    foreach(UIElement* e, elements) {
        delete e;
    }
    elements.clear();
}

void UILayout::setHorizontal(bool horizontal) {
    this->horizontal = horizontal;
};

void UILayout::setUI(UI* ui) {
    this->ui = ui;
    foreach(UIElement* e, elements) {
        e->setUI(ui);
    }
}

void UILayout::drawBackground() {
    if(bgcolour.w <= 0.0f) return;

    glColor4fv(glm::value_ptr(bgcolour));

    glDisable(GL_TEXTURE_2D);
    drawQuad(pos, rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glEnable(GL_TEXTURE_2D);
}

void UILayout::addElement(UIElement* e) {
    e->parent = this;
    if(ui!=0) e->setUI(ui);

    elements.push_back(e);
}

void UILayout::update(float dt) {

    rect = min_rect;

    vec2 inner = vec2(0.0f, 0.0f);

    int visible_elements = 0;

    std::list<UIElement*> fill_elements;

    foreach(UIElement* e, elements) {
        e->resetRect();
        e->update(dt);

        if(!e->hidden) {
            visible_elements++;

            if(e->fill) fill_elements.push_back(e);

            vec2 r = e->getRect();

            if(horizontal) {
                inner.x += r.x;
                inner.y = std::max(inner.y, r.y);
            } else {
                inner.x = std::max(inner.x, r.x);
                inner.y += r.y;
            }
        }
    }

    if(horizontal) {
        inner.x += margin.x*2.0f + ((float)visible_elements-1) * padding.x;
        inner.y += margin.y*2.0f;
    } else {
        inner.x += margin.x*2.0f;
        inner.y += margin.y*2.0f + ((float)visible_elements-1) * padding.y;
    }

    rect = glm::max(rect, inner);

    if(!fill_elements.empty()) {

        vec2 filler = glm::max(vec2(0.0f), vec2(rect-inner)) / (float) fill_elements.size();

        foreach(UIElement* e, fill_elements) {
            e->expandRect(filler);
            e->update(0.0f);
        }
    }
}

void UILayout::setMinRect(const vec2& min_rect) {
    this->min_rect = min_rect;
}

void UILayout::updatePos(const vec2& pos) {

    switch(alignment) {
        case UI_LAYOUT_ALIGN_TOP_LEFT:
            this->pos = vec2(0.0f, 0.0f);
            break;
        case UI_LAYOUT_ALIGN_BOTTOM_LEFT:
            this->pos = vec2(0.0f, display.height - rect.y);
            break;
        case UI_LAYOUT_ALIGN_TOP_RIGHT:
            this->pos = vec2(display.width-rect.x, 0.0f);
            break;
        case UI_LAYOUT_ALIGN_BOTTOM_RIGHT:
            this->pos = vec2(display.width-rect.x, display.height - rect.y);
            break;
        default:
            this->pos = pos;
            break;
    }

    bool right_align = (   alignment == UI_LAYOUT_ALIGN_TOP_RIGHT
                        || alignment == UI_LAYOUT_ALIGN_BOTTOM_RIGHT) ?
                       true : false;

    vec2 cursor;

    if(right_align) {
        cursor = this->pos + vec2(rect.x - margin.x, margin.y);
    } else {
        cursor = this->pos + margin.xy;
    }

    foreach(UIElement* e, elements) {

        if(e->hidden) continue;

        vec2 r = e->getRect();

        if(centre) {
            if(horizontal) {
                e->updatePos(vec2(cursor.x, (int)(this->pos.y + rect.y*0.5f - r.y*0.5f)));
            } else {
                e->updatePos(vec2((int)(this->pos.x + rect.x*0.5f - r.x*0.5f), cursor.y));
            }
        } else if(right_align) {
            e->updatePos(cursor - vec2(r.x, 0.0f));
        } else {
            e->updatePos(cursor);
        }

        if(horizontal) {
            cursor.x += r.x + padding.x;
        } else {
            cursor.y += r.y + padding.y;
        }
    }

}

bool UILayout::elementsByType(std::list<UIElement*>& found, int type) {

    bool success = UIElement::elementsByType(found, type);

    foreach(UIElement* e, elements) {
        if(e->elementsByType(found, type)) success = true;
    }

    return success;
}

UIElement* UILayout::elementAt(const vec2& pos) {

    if(hidden) return 0;

    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) return found;
    }

    return UIElement::elementAt(pos);
}

void UILayout::drawOutline() {

    UIElement::drawOutline();

    foreach(UIElement* e, elements) {
        if(!e->hidden) e->drawOutline();
    }
}

void UILayout::draw() {

    drawBackground();

    foreach(UIElement* e, elements) {
        if(!e->hidden) e->draw();
    }

}

//UILabelledElement

UILabelledElement::UILabelledElement(const std::string text, UIElement* e, float width) : UILayout(true) {
    addElement(new UILabel(text, false, width));
    addElement(e);
}

//UIResizableLayout

UIResizableLayout::UIResizableLayout(bool horizontal) : UILayout(horizontal) {
    resize_button = new UIResizeButton();
    resize_button->parent = this;
}

UIResizableLayout::~UIResizableLayout() {
    if(resize_button != 0) delete resize_button;
}

UIElement* UIResizableLayout::elementAt(const vec2& pos) {

    if(hidden) return 0;

    UIElement* found = 0;

    if((found = resize_button->elementAt(pos)) != 0) return found;

    return UILayout::elementAt(pos);
}


void UIResizableLayout::setUI(UI* ui) {
    UILayout::setUI(ui);
    resize_button->setUI(ui);
}

void UIResizableLayout::updatePos(const vec2& pos) {
    UILayout::updatePos(pos);
    resize_button->updatePos(pos + rect - resize_button->getRect());
}

void UIResizableLayout::update(float dt) {
    UILayout::update(dt);
    resize_button->update(dt);
}

void UIResizableLayout::draw() {
    UILayout::draw();
    resize_button->draw();
}

//UIResizeButton

UIResizeButton::UIResizeButton() {

    resizetex = texturemanager.grab("ui/resize.png", false);
    resizetex->bind();
    resizetex->setFiltering(GL_NEAREST, GL_NEAREST);
    resizetex->setWrapStyle(GL_CLAMP);

    rect = vec2(16.0f, 16.0f);
}

UIResizeButton::~UIResizeButton() {
    if(resizetex!=0) texturemanager.release(resizetex);
}

void UIResizeButton::drag(const vec2& pos) {

    vec2 resize_pos = pos;

    if(resize_pos.x > this->pos.x && resize_pos.y > this->pos.y)
        resize_pos = glm::max(this->pos+rect, resize_pos);

    ((UILayout*)parent)->setMinRect(glm::abs(parent->pos - resize_pos));
}

void UIResizeButton::drawContent() {
    resizetex->bind();
    drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
}
