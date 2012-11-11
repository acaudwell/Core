#include "layout.h"
#include "label.h"

UILayout::UILayout(bool horizontal) : horizontal(horizontal), UIElement() {
    alignment = UI_LAYOUT_ALIGN_NONE;
    bgcolour  = vec4(0.0f);
    min_rect  = vec2(0.0f);
    centre    = false;
    drawbg    = false;
    selectable = false;
    expanded_rect = vec2(0.0f);
}

UILayout::~UILayout() {
    clear();
}

void UILayout::setDrawBackground(bool drawbg) {
    this->drawbg = drawbg;
}

void UILayout::clear() {
    for(UIElement* e: elements) {
        delete e;
    }
    elements.clear();
}

void UILayout::setHorizontal(bool horizontal) {
    this->horizontal = horizontal;
};

void UILayout::setUI(UI* ui) {
    this->ui = ui;
    for(UIElement* e: elements) {
        e->setUI(ui);
    }
}

void UILayout::drawBackground() {
    if(!drawbg) return;

    if(bgcolour.w > 0.0f) {
        glColor4fv(glm::value_ptr(bgcolour));
    } else {
        glColor4fv(glm::value_ptr(ui->getBackgroundColour()));
    }

    ui->setTextured(false);
    drawQuad(pos, getRect(), vec4(0.0f, 0.0f, 1.0f, 1.0f));
    ui->setTextured(true);
}

void UILayout::addElement(UIElement* e) {
    e->parent = this;
    if(ui!=0) e->setUI(ui);

    elements.push_back(e);
}

void UILayout::update(float dt) {

    updateZIndex();

    vec2 inner = vec2(0.0f, 0.0f);

    int visible_elements = 0;

    std::list<UIElement*> fill_vert_elements;
    std::list<UIElement*> fill_horiz_elements;

    for(UIElement* e: elements) {
        e->resetRect();
        e->update(dt);

        if(e->isVisible()) {
            visible_elements++;

            if(e->fillHorizontal()) fill_horiz_elements.push_back(e);
            if(e->fillVertical())   fill_vert_elements.push_back(e);

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

    vec4 margin = getMargin();

    if(horizontal) {
        inner.x += margin.x+margin.z + ((float)visible_elements-1) * padding.x;
        inner.y += margin.y+margin.w;
    } else {
        inner.x += margin.x+margin.z;
        inner.y += margin.y+margin.w + ((float)visible_elements-1) * padding.y;
    }

    rect = glm::max(min_rect, inner);

    if(fill_vert_elements.empty() && fill_horiz_elements.empty()) return;

    vec2 filler = glm::max(vec2(0.0f), vec2(rect-inner)) + expanded_rect;

    if(horizontal && !fill_horiz_elements.empty()) {
        filler.x /= (float) fill_horiz_elements.size();
    } else if(!fill_vert_elements.empty()) {
        filler.y /= (float) fill_vert_elements.size();
    }

    std::list<UIElement*> fill_elements;
    fill_elements.insert(fill_elements.end(), fill_horiz_elements.begin(), fill_horiz_elements.end());
    fill_elements.insert(fill_elements.end(), fill_vert_elements.begin(), fill_vert_elements.end());

    fill_elements.unique();

    for(UIElement* e: fill_elements) {

        vec2 efill(0.0f);

        if(e->fillHorizontal()) {
            if(!horizontal) efill.x = filler.x + glm::max(0.0f, inner.x - e->rect.x - margin.x - margin.z);
            else efill.x = filler.x;
        }

        if(e->fillVertical()) {
            if(horizontal) efill.y = filler.y + glm::max(0.0f, inner.y - e->rect.y - margin.y - margin.w);
            else efill.y = filler.y;
        }

        if(efill.x > 0.0f || efill.y > 0.0f) {
            e->expandRect(efill);
            e->update(0.0f);
        }
    }

    if(!elements.empty() && elements.front()->getType() == UI_SELECT) debugLog("first element is a select");
}

void UILayout::expandRect(const vec2& expand) {
    expanded_rect = expand;
}

void UILayout::resetRect() {
    expanded_rect = vec2(0.0f);
}

vec2 UILayout::getInnerRect() {
    return rect;
}

vec2 UILayout::getRect() {
    return rect + expanded_rect;
}

void UILayout::setMinRect(const vec2& min_rect) {
    this->min_rect = min_rect;
}

void UILayout::updatePos(const vec2& pos) {

    vec2 rect = getRect();

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

    bool bottom_align   = (   alignment == UI_LAYOUT_ALIGN_BOTTOM_LEFT
                           || alignment == UI_LAYOUT_ALIGN_BOTTOM_RIGHT) ?
                       true : false;
    vec2 cursor;

    vec4 margin = getMargin();

    cursor = this->pos + vec2( (right_align  ? rect.x - margin.z : margin.x) ,
                               (bottom_align ? rect.y - margin.w : margin.y) );

    for(UIElement* e: elements) {

        if(!e->isVisible()) continue;

        vec2 r = e->getRect();

        if(centre) {
            if(horizontal) {
                e->updatePos(vec2(cursor.x, (int)(this->pos.y + rect.y*0.5f - r.y*0.5f)));
            } else {
                e->updatePos(vec2((int)(this->pos.x + rect.x*0.5f - r.x*0.5f), cursor.y));
            }
        } else {
            e->updatePos(cursor - vec2(right_align ? r.x : 0.0, bottom_align ? r.y : 0.0f));
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

    for(UIElement* e: elements) {
        if(e->elementsByType(found, type)) success = true;
    }

    return success;
}

void UILayout::elementsAt(const vec2& pos, std::list<UIElement*>& elements_found) {

    if(!isVisible()) return;

    UIElement* found = 0;

    for(UIElement* e: elements) {
        e->elementsAt(pos, elements_found);
    }

    UIElement::elementsAt(pos, elements_found);
}

void UILayout::resize(const vec2& pos) {
    setMinRect(glm::abs(this->pos - pos));
}

void UILayout::drawOutline() {

    UIElement::drawOutline();

    for(UIElement* e: elements) {
        if(e->isVisible()) e->drawOutline();
    }
}

void UILayout::draw() {

    drawBackground();

    for(UIElement* e: elements) {
        if(e->isVisible()) e->draw();
    }

}

//UILabelledElement

UILabelledElement::UILabelledElement(const std::string text, UIElement* e, float width) : UILayout(true) {

    label = new UILabel(text, false, width);

    if(width<0.0f) label->setFillHorizontal(true);

    layout = new UILayout(true);
    layout->setPadding(5.0f);
    layout->setMinRect(vec2(213.0f, 0.0f));

    addElement(label);
    addElement(layout);

    setFillHorizontal(true);

    if(e != 0) layout->addElement(e);
}

UILabel* UILabelledElement::getLabel() {
    return label;
}

UILayout* UILabelledElement::getLayout() {
    return layout;
}


//UIResizableLayout

UIResizableLayout::UIResizableLayout(bool horizontal) : UILayout(horizontal) {
    resize_button = new UIResizeButton();
    resize_button->parent = this;
}

UIResizableLayout::~UIResizableLayout() {
    if(resize_button != 0) delete resize_button;
}

void UIResizableLayout::elementsAt(const vec2& pos, std::list<UIElement*>& elements_found) {

    if(!isVisible()) return;

    resize_button->elementsAt(pos, elements_found);

    return UILayout::elementsAt(pos, elements_found);
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

    ((UILayout*)parent)->resize(resize_pos);
}

void UIResizeButton::drawContent() {
    resizetex->bind();
    glColor4fv(glm::value_ptr(ui->getSolidColour()));
    drawQuad(rect, vec4(0.0f, 0.0f, 1.0f, 1.0f));
}
