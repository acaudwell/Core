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

#ifndef SDLAPP_MOUSECURSOR_H
#define SDLAPP_MOUSECURSOR_H

#include "display.h"

class MouseCursor {

    vec2 mousepos;
    vec2 rel;

    bool hidden;
    bool system_cursor;

    float idle;
    float timeout;

    int scrollwheel;

    SDL_Cursor* sdl_default_cursor;
    SDL_Cursor* sdl_hidden_cursor;

    bool left_click;
    bool right_click;
    bool middle_click;

    TextureResource* cursortex;
public:
    MouseCursor();
    ~MouseCursor();

    const vec2& getPos() const;
    const vec2& getRelativePos() const;

    void leftClick(bool click);
    void rightClick(bool click);
    void middleClick(bool click);

    bool leftClick() const;
    bool rightClick() const;
    bool middleClick() const;

    bool leftButtonPressed() const;
    bool rightButtonPressed() const;
    bool bothPressed() const;
    bool buttonPressed() const;

    int scrollWheel() const;

    bool isHidden() const;
    bool isSystemCursor()const;
    bool isVisible() const;
    bool hasFocus() const;

    void scroll(bool dir);

    void resetButtonState();

    void updateRelativePos(const vec2& rel);
    void updatePos(const vec2& pos);

    void showCursor(bool show);
    void useSystemCursor(bool system_cursor);

    void setCursorTexture(TextureResource* texture);

    void logic(float dt);
    void draw() const;
};

#endif
