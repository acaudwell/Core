#include "layout.h"

UILayout::UILayout(bool horizontal) : horizontal(horizontal), UIElement() {
    alignment = UI_LAYOUT_ALIGN_NONE;

}

void UILayout::setHorizontal(bool horizontal) {
    this->horizontal = horizontal;
};

void UILayout::setUI(UI* ui) {
    foreach(UIElement* e, elements) {
        e->setUI(ui);
    }
}

void UILayout::addElement(UIElement* e) {
    e->parent = this;
    elements.push_back(e);
}

void UILayout::update(float dt) {

    rect = vec2(0.0f, 0.0f);

    foreach(UIElement* e, elements) {
        e->update(dt);

        if(horizontal) {
            rect.x += e->rect.x;
            rect.y = std::max(rect.y, e->rect.y);
        } else {
            rect.x = std::max(rect.x, e->rect.x);
            rect.y += e->rect.y;
        }
    }

    if(horizontal) {
        rect.x += margin.x*2.0f + ((float)elements.size()-1.0f) * padding.x;
        rect.y += margin.y*2.0f;
    } else {
        rect.x += margin.x*2.0f;
        rect.y += margin.y*2.0f + ((float)elements.size()-1.0f) * padding.y;
    }
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

        if(right_align) {
            e->updatePos(cursor - vec2(e->rect.x, 0.0f));
        } else {
            e->updatePos(cursor);
        }

        if(horizontal) {
            cursor.x += e->rect.x + padding.x;
        } else {
            cursor.y += e->rect.y + padding.y;
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

    if(!UIElement::elementAt(pos)) return 0;

    UIElement* found = 0;

    foreach(UIElement* e, elements) {
        if((found = e->elementAt(pos)) != 0) return found;
    }

    return 0;
}

void UILayout::drawOutline() {

    UIElement::drawOutline();

    foreach(UIElement* e, elements) {
        e->drawOutline();
    }
}

void UILayout::draw() {
    foreach(UIElement* e, elements) {
        e->draw();
    }

}
