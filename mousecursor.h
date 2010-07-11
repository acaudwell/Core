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

    vec2f mousepos;

    bool hidden;
    bool system_cursor;

    float idle;
    float timeout;

    TextureResource* cursortex;
public:
    MouseCursor();

    vec2f getPos() const { return mousepos; }

    bool leftButtonPressed() const;
    bool rightButtonPressed() const;
    bool bothPressed() const;
    bool buttonPressed() const;

    bool isHidden() const;
    bool isSystemCursor()const;
    bool isVisible() const;
    bool hasFocus() const;

    void updatePos(const vec2f& pos);

    void showCursor(bool show);
    void useSystemCursor(bool system_cursor);

    void setCursorTexture(TextureResource* texture);

    void logic(float dt);
    void draw() const;
};

#endif
