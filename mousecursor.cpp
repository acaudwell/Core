/*
    Copyright (c) 2010 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "mousecursor.h"

MouseCursor::MouseCursor() {
    system_cursor = true;
    cursortex     = 0;
    hidden        = false;
    timeout       = 3.0f;
    idle          = timeout;
    scrollwheel   = 0;

    left_click   = false;
    right_click  = false;
    middle_click = false;

    sdl_default_cursor = SDL_GetCursor();


    Uint8 data[4*32];
    Uint8 mask[4*32];

    memset(data, 0, sizeof(data));
    memset(mask, 0, sizeof(mask));

    sdl_hidden_cursor = SDL_CreateCursor(data, mask, 32, 32, 0, 0);
}

MouseCursor::~MouseCursor() {
    SDL_SetCursor(sdl_default_cursor);
    SDL_FreeCursor(sdl_hidden_cursor);
}

void MouseCursor::useSystemCursor(bool system_cursor) {
    this->system_cursor = system_cursor;

    if(!hidden) {
        if(system_cursor) SDL_SetCursor(sdl_default_cursor);
        else SDL_SetCursor(sdl_hidden_cursor);
    } else {
        SDL_SetCursor(sdl_hidden_cursor);
    }
}

void MouseCursor::showCursor(bool show) {
    if(this->hidden == !show) return;

    this->hidden = !show;
    if(show) idle = 0.0;

    if(system_cursor) {
        if(show) SDL_SetCursor(sdl_default_cursor);
        else SDL_SetCursor(sdl_hidden_cursor);
    }
}
const vec2& MouseCursor::getPos() const {
    return mousepos;
}

const vec2& MouseCursor::getRelativePos() const {
    return rel;
}

void MouseCursor::updateRelativePos(const vec2& rel) {
    this->rel = rel;
}

void MouseCursor::leftClick(bool click) {
    left_click = click;
}

void MouseCursor::rightClick(bool click) {
    right_click = click;
}

void MouseCursor::middleClick(bool click) {
    middle_click = click;
}

bool MouseCursor::leftClick() const {
    return left_click;
}

bool MouseCursor::rightClick() const {
    return right_click;
}

bool MouseCursor::middleClick() const {
    return middle_click;
}

bool MouseCursor::leftButtonPressed() const {
    Uint8 ms = SDL_GetMouseState(0,0);
    return ms & SDL_BUTTON(SDL_BUTTON_LEFT);
}

bool MouseCursor::rightButtonPressed() const {
    Uint8 ms = SDL_GetMouseState(0,0);
    return ms & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

bool MouseCursor::bothPressed() const {
    Uint8 ms = SDL_GetMouseState(0,0);
    return (ms & SDL_BUTTON(SDL_BUTTON_RIGHT) && ms & SDL_BUTTON(SDL_BUTTON_LEFT));
}

bool MouseCursor::buttonPressed() const {
    Uint8 ms = SDL_GetMouseState(0,0);
    return (ms & SDL_BUTTON(SDL_BUTTON_RIGHT) || ms & SDL_BUTTON(SDL_BUTTON_LEFT));
}

int MouseCursor::scrollWheel() const {
    return scrollwheel;
}

bool MouseCursor::isSystemCursor() const {
    return system_cursor;
}

bool MouseCursor::isHidden() const {
    return hidden;
}

bool MouseCursor::isVisible() const {
    return (!hidden && idle < timeout && hasFocus());
}

bool MouseCursor::hasFocus() const {
#if SDL_VERSION_ATLEAST(2,0,0)
    return (SDL_GetMouseFocus() == display.sdl_window);
#else
    return (SDL_GetAppState() & SDL_APPMOUSEFOCUS);
#endif
}

void MouseCursor::setCursorTexture(TextureResource* texture) {
    cursortex = texture;
}

void MouseCursor::updatePos(const vec2& pos) {
    mousepos = pos;
    idle     = 0.0f;
}

void MouseCursor::scroll(bool dir) {
    scrollwheel += dir ? 1 : -1;
}

void MouseCursor::resetButtonState() {
    scrollwheel  = 0;
    right_click  = false;
    left_click   = false;
    middle_click = false;
}

void MouseCursor::logic(float dt) {
    idle += dt;
}

void MouseCursor::draw() const {
    if(system_cursor || cursortex == 0) return;
    if(!isVisible()) return;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    cursortex->bind();

    glTranslatef(mousepos.x, mousepos.y, 0.0f);

    glPushMatrix();
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);
        glVertex2i(0, 0);

        glTexCoord2f(1.0f,0.0f);
        glVertex2i(cursortex->w, 0);

        glTexCoord2f(1.0f,1.0f);
        glVertex2i(cursortex->w, cursortex->h);

        glTexCoord2f(0.0f,1.0f);
        glVertex2i(0, cursortex->h);
    glEnd();

    glPopMatrix();
}
