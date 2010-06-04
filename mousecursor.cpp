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
    visible       = true;
    grabbed       = false;
}

void MouseCursor::useSystemCursor(bool system_cursor) {
    this->system_cursor = system_cursor;

    if(visible) {
        if(system_cursor) SDL_ShowCursor(true);
        else SDL_ShowCursor(false);
    } else {
        SDL_ShowCursor(false);
    }
}

void MouseCursor::showCursor(bool visible) {
    if(this->visible == visible) return;

    this->visible = visible;

    if(system_cursor) {
        if(visible) SDL_ShowCursor(true);
        else SDL_ShowCursor(false);
    }
}

bool MouseCursor::leftButtonPressed() {
    Uint8 ms = SDL_GetMouseState(0,0);
    return ms & SDL_BUTTON(SDL_BUTTON_LEFT);
}

bool MouseCursor::rightButtonPressed() {
    Uint8 ms = SDL_GetMouseState(0,0);
    return ms & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

bool MouseCursor::bothPressed() {
    Uint8 ms = SDL_GetMouseState(0,0);
    return (ms & SDL_BUTTON(SDL_BUTTON_RIGHT) && ms & SDL_BUTTON(SDL_BUTTON_LEFT));
}

bool MouseCursor::buttonPressed() {
    Uint8 ms = SDL_GetMouseState(0,0);
    return (ms & SDL_BUTTON(SDL_BUTTON_RIGHT) || ms & SDL_BUTTON(SDL_BUTTON_LEFT));
}

bool MouseCursor::isSystemCursor() {
    return system_cursor;
}

bool MouseCursor::isVisible() {
    return visible;
}

bool MouseCursor::isGrabbed() {
    return grabbed;
}

void MouseCursor::grab() {
    SDL_WM_GrabInput(SDL_GRAB_ON);
    showCursor(false);
    grabbed=true;
}

void MouseCursor::ungrab() {
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    showCursor(true);
    grabbed=false;
}

void MouseCursor::setCursorTexture(TextureResource* texture) {
    cursortex = texture;
}

void MouseCursor::draw(const vec2f& pos) {
    if(system_cursor || cursortex == 0) return;
    if(!visible) return;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, cursortex->textureid);

    glTranslatef(pos.x, pos.y, 0.0f);

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
